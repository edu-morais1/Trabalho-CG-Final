import tkinter as tk
from tkinter import ttk
import numpy as np
from PIL import Image, ImageTk
import ctypes
import sys
import platform

# Carregar biblioteca C++
if platform.system() == 'Linux':
    lib = ctypes.CDLL('./librender.so')
elif platform.system() == 'Windows':
    lib = ctypes.CDLL('./render.dll')
else:  # macOS
    lib = ctypes.CDLL('./librender.dylib')

# Definir assinatura da função
lib.render_api.argtypes = [
    ctypes.c_int, ctypes.c_int,  # width, height
    ctypes.c_double, ctypes.c_double, ctypes.c_double,  # eye
    ctypes.c_double, ctypes.c_double, ctypes.c_double,  # center
    ctypes.c_double, ctypes.c_double, ctypes.c_double,  # fov, near, far
    ctypes.c_int, ctypes.POINTER(ctypes.c_double),  # num_cubes, cubes_data
    ctypes.c_int, ctypes.POINTER(ctypes.c_double),  # num_lights, lights_data
    ctypes.c_int,  # use_phong
    ctypes.POINTER(ctypes.c_uint32)  # out_pixels
]

class RenderApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Trabalho CG - Modelador 3D")
        self.root.geometry("1400x800")
        
        # Configurações
        self.width = 800
        self.height = 600
        self.selected_cube = 0
        self.use_phong = tk.BooleanVar(value=True)
        
        # Estado da cena
        self.camera = {
            'eye': [3.0, 2.0, 4.0],
            'center': [0.0, 0.0, 0.0],
            'fov': 60.0,
            'near': 0.1,
            'far': 100.0
        }
        
        self.cubes = [
            {'pos': [-1.5, 0, 0], 'rot': [0, 0.5, 0], 'scale': 1.0, 
             'color': [0.8, 0.2, 0.2], 'ka': 0.1, 'kd': 0.7, 'ks': 0.3},
            {'pos': [1.5, 0, 0], 'rot': [0.3, 0, 0], 'scale': 1.0,
             'color': [0.2, 0.8, 0.2], 'ka': 0.1, 'kd': 0.7, 'ks': 0.3},
            {'pos': [0, 1.5, 0], 'rot': [0, 0, 0.3], 'scale': 0.8,
             'color': [0.2, 0.2, 0.8], 'ka': 0.1, 'kd': 0.7, 'ks': 0.5}
        ]
        
        self.lights = [
            {'pos': [5, 5, 5], 'color': [1, 1, 1], 'intensity': 1.0},
            {'pos': [-3, 2, 3], 'color': [0.5, 0.5, 0.8], 'intensity': 0.5}
        ]
        
        self.setup_ui()
        self.render()
    
    def setup_ui(self):
        # Layout principal
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Canvas de renderização
        canvas_frame = ttk.LabelFrame(main_frame, text="Visualização", padding=10)
        canvas_frame.grid(row=0, column=0, sticky='nsew', padx=5)
        
        self.canvas = tk.Canvas(canvas_frame, width=self.width, height=self.height, bg='black')
        self.canvas.pack()
        
        # Painel de controles
        controls_frame = ttk.Frame(main_frame)
        controls_frame.grid(row=0, column=1, sticky='nsew', padx=5)
        
        # Câmera
        cam_frame = ttk.LabelFrame(controls_frame, text="Câmera", padding=10)
        cam_frame.pack(fill=tk.X, pady=5)
        
        self.create_slider(cam_frame, "Eye X", self.camera['eye'], 0, -10, 10)
        self.create_slider(cam_frame, "Eye Y", self.camera['eye'], 1, -10, 10)
        self.create_slider(cam_frame, "Eye Z", self.camera['eye'], 2, -10, 10)
        self.create_slider(cam_frame, "FOV", self.camera, 'fov', 30, 120)
        
        # Seleção de cubo
        cube_frame = ttk.LabelFrame(controls_frame, text="Cubo", padding=10)
        cube_frame.pack(fill=tk.X, pady=5)
        
        ttk.Button(cube_frame, text="Cubo 1", 
                  command=lambda: self.select_cube(0)).grid(row=0, column=0, padx=2)
        ttk.Button(cube_frame, text="Cubo 2",
                  command=lambda: self.select_cube(1)).grid(row=0, column=1, padx=2)
        ttk.Button(cube_frame, text="Cubo 3",
                  command=lambda: self.select_cube(2)).grid(row=0, column=2, padx=2)
        ttk.Button(cube_frame, text="+ Adicionar",
                  command=self.add_cube).grid(row=0, column=3, padx=2)
        
        # Transformações
        self.transform_frame = ttk.LabelFrame(controls_frame, text="Transformações", padding=10)
        self.transform_frame.pack(fill=tk.X, pady=5)
        self.update_cube_controls()
        
        # Shading
        shading_frame = ttk.LabelFrame(controls_frame, text="Renderização", padding=10)
        shading_frame.pack(fill=tk.X, pady=5)
        
        ttk.Checkbutton(shading_frame, text="Phong Shading", 
                       variable=self.use_phong, command=self.render).pack()
        ttk.Button(shading_frame, text="🔄 Renderizar", 
                  command=self.render).pack(pady=5)
        
        # Configurar grid
        main_frame.columnconfigure(0, weight=3)
        main_frame.columnconfigure(1, weight=1)
    
    def create_slider(self, parent, label, data, key, min_val, max_val):
        frame = ttk.Frame(parent)
        frame.pack(fill=tk.X, pady=2)
        
        ttk.Label(frame, text=label, width=10).pack(side=tk.LEFT)
        
        if isinstance(key, int):
            val = tk.DoubleVar(value=data[key])
            def update(v): data[key] = float(v); self.render()
        else:
            val = tk.DoubleVar(value=data[key])
            def update(v): data[key] = float(v); self.render()
        
        slider = ttk.Scale(frame, from_=min_val, to=max_val, 
                          variable=val, command=update)
        slider.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        
        label_val = ttk.Label(frame, textvariable=val, width=6)
        label_val.pack(side=tk.LEFT)
    
    def select_cube(self, idx):
        self.selected_cube = idx
        self.update_cube_controls()
    
    def update_cube_controls(self):
        # Limpar controles antigos
        for widget in self.transform_frame.winfo_children():
            widget.destroy()
        
        cube = self.cubes[self.selected_cube]
        
        self.create_slider(self.transform_frame, "Pos X", cube['pos'], 0, -5, 5)
        self.create_slider(self.transform_frame, "Pos Y", cube['pos'], 1, -5, 5)
        self.create_slider(self.transform_frame, "Pos Z", cube['pos'], 2, -5, 5)
        self.create_slider(self.transform_frame, "Rot X", cube['rot'], 0, -3.14, 3.14)
        self.create_slider(self.transform_frame, "Rot Y", cube['rot'], 1, -3.14, 3.14)
        self.create_slider(self.transform_frame, "Rot Z", cube['rot'], 2, -3.14, 3.14)
        self.create_slider(self.transform_frame, "Escala", cube, 'scale', 0.1, 3)
    
    def add_cube(self):
        self.cubes.append({
            'pos': [0, 0, 0], 'rot': [0, 0, 0], 'scale': 1.0,
            'color': [0.5, 0.5, 0.5], 'ka': 0.1, 'kd': 0.7, 'ks': 0.3
        })
        self.render()
    
    def render(self, *args):
        # Preparar dados
        cubes_data = []
        for cube in self.cubes:
            cubes_data.extend(cube['pos'])
            cubes_data.extend(cube['rot'])
            cubes_data.append(cube['scale'])
            cubes_data.extend(cube['color'])
            cubes_data.extend([cube['ka'], cube['kd'], cube['ks']])
        
        lights_data = []
        for light in self.lights:
            lights_data.extend(light['pos'])
            lights_data.extend(light['color'])
            lights_data.append(light['intensity'])
        
        # Arrays ctypes
        cubes_arr = (ctypes.c_double * len(cubes_data))(*cubes_data)
        lights_arr = (ctypes.c_double * len(lights_data))(*lights_data)
        pixels = (ctypes.c_uint32 * (self.width * self.height))()
        
        # Chamar C++
        lib.render_api(
            self.width, self.height,
            *self.camera['eye'], *self.camera['center'],
            self.camera['fov'], self.camera['near'], self.camera['far'],
            len(self.cubes), cubes_arr,
            len(self.lights), lights_arr,
            int(self.use_phong.get()),
            pixels
        )
        
        # Converter para imagem
        img_data = np.array(pixels, dtype=np.uint32).reshape((self.height, self.width))
        img_rgb = np.zeros((self.height, self.width, 3), dtype=np.uint8)
        img_rgb[:,:,0] = (img_data >> 16) & 0xFF  # R
        img_rgb[:,:,1] = (img_data >> 8) & 0xFF   # G
        img_rgb[:,:,2] = img_data & 0xFF          # B
        
        # Exibir no canvas
        img = Image.fromarray(img_rgb)
        self.photo = ImageTk.PhotoImage(img)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo)

if __name__ == '__main__':
    root = tk.Tk()
    app = RenderApp(root)
    root.mainloop()

