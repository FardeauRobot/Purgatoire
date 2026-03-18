# Rendering in Cub3D: The Single Image Buffer Approach

Welcome to the rendering guide for your Cub3D project! 

The most efficient way to render frames using MiniLibX (especially for raycasting) is to avoid `mlx_pixel_put` or sending multiple images to the window. Instead, we use a technique called **Double Buffering** (or Single Image Buffering in this context). 

We create one large image the exact size of your window, modify its pixels directly in memory, and then push the entire image to the window exactly once per frame.

---

## 🏗️ The Generic Image Struct
In MiniLibX, every image—whether it's your 1920x1080 main screen buffer or a 64x64 wall texture loaded from an `.xpm` file—has the exact same underlying memory structure. 

By creating a generic `t_img` struct, you can use the exact same functions for everything:

```c
typedef struct s_img {
    void    *img;          // Pointer from mlx_new_image or mlx_xpm_file...
    char    *addr;         // The memory address from mlx_get_data_addr
    int     bits_per_pixel;
    int     line_length;
    int     endian;
    int     width;         // Very useful for textures (e.g., 64)
    int     height;        // Very useful for textures (e.g., 64)
} t_img;
```

---

## 🎨 Reading and Writing Pixels

### 1. The Generic Put Pixel (For writing to the screen)
Since `mlx_pixel_put` is very slow, we write directly to the `img.addr` memory.

```c
void my_mlx_pixel_put(t_img *data, int x, int y, int color)
{
    char    *dst;

    // Boundary check to prevent segfaults!
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return ;

    // Calculate the exact memory offset for the coordinates (x, y)
    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    
    // Write the color
    *(unsigned int*)dst = color;
}
```

### 2. The Generic Get Pixel (For reading from textures)
When you load an `.xpm` file, you need to read its pixels to map them onto your 3D walls.

```c
unsigned int get_pixel(t_img *img, int x, int y)
{
    char    *pixel;

    // Safety bounds! Prevents reading outside texture memory
    if (x < 0 || x >= img->width || y < 0 || y >= img->height)
        return (0); // Return black or transparent if out of bounds
        
    // Calculate the exact memory offset
    pixel = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
    
    // Read the color integer and return it
    return (*(unsigned int *)pixel);
}
```

### 3. How to use them together
To draw a textured wall on your screen, you simply read from the texture and write to the screen:
```c
// 1. READ from the generic texture struct
unsigned int color = get_pixel(&game->tex_north, tex_x, tex_y);

// 2. WRITE to the generic screen struct
my_mlx_pixel_put(&game->screen, screen_x, screen_y, color);
```

---

## ⚡ Performance: The Blazingly Fast Way
Using `get_pixel` and `put_pixel` is great for UI and the minimap. However, calling these functions for *every single pixel* during the 3D raycasting of a 1080p window can be too slow because of function overhead and math multiplications.

**Optimization:** For the vertical wall stripes in your raycaster, bypass the functions and use **Direct Memory Access**. 

Since a wall stripe keeps the same `x` and only the `y` goes down, you can calculate the memory address of the top pixel, and simply jump forward by `line_length` for each pixel below it!

```c
// Calculate how many integers make up one line (pitch)
int screen_pitch = screen->line_length / 4;
int tex_pitch = texture->line_length / 4;

// Cast the raw char* to unsigned int* arrays so we can read/write colors directly
unsigned int *screen_pixels = (unsigned int *)screen->addr;
unsigned int *tex_pixels = (unsigned int *)texture->addr;

// Find the precise memory index for the very top pixel of our wall stripe
int screen_index = (draw_start * screen_pitch) + x;

int y = draw_start;
while (y <= draw_end)
{
    // Calculate exact texture Y coordinate
    int tex_y = (int)tex_pos & (texture->height - 1);
    tex_pos += step;
    
    // FAST READ & WRITE: Direct array access!
    unsigned int color = tex_pixels[(tex_y * tex_pitch) + tex_x];
    screen_pixels[screen_index] = color;
    
    // Jump directly to the pixel right underneath for the next loop!
    screen_index += screen_pitch; 
    
    y++;
}
```

---

## 🗺️ Implementing the Minimap

With a single image buffer, the minimap is incredibly easy! Draw it **after** you draw the 3D world, but **before** you push the image to the window. It will cleanly overwrite the 3D scene in the corner.

**Solid Colors vs Textures:**
*   **Solid Colors:** Just loop through coordinates and call `my_mlx_pixel_put(..., color)`. You don't need a separate image struct just to hold a solid color!
*   **Textured Minimap:** Loop through coordinates, call `get_pixel` from your wall texture, and `my_mlx_pixel_put` to the screen.

---

## 🧼 Advice for Readable & Clean Code

Raycasting math can get ugly fast. Here are tips to keep it maintainable, especially considering the 42 Norm (Norminette):

### 1. Single Responsibility Principle
Keep functions under 25 lines (as per Norm) by making them do **exactly one thing**.
*   `render_frame()`: Calls the functions below.
*   `draw_ceiling_and_floor()`: Fills the background.
*   `cast_rays()`: A loop that calls `cast_single_ray(x)` for each vertical strip.
*   `cast_single_ray(x)`: Handles math for one column.
*   `calculate_step_and_side_dist()`: Setup DDA algorithm.
*   `perform_dda()`: Executes the DDA loop.
*   `draw_wall_strip(x)`: Calculates line height and writes pixels.

### 2. Self-Documenting Variables
Don't use `a`, `b`, `c`. 
Use `wall_height`, `draw_start`, `draw_end`, `step_x`. Even if the math tutorials use standard letters, rename them in your code so you know exactly what they represent 2 weeks later.

### 3. Separate Math from Rendering
Keep your raycasting calculations in separate files from your MiniLibX code. This makes it easier to debug the math without worrying if the pixel-putting is wrong.
