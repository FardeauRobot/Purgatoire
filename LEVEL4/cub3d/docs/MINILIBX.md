### In ft_pixel_draw (render_utils.c): 

	dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
	*(unsigned int *)dst = color;

Those 2 lines write one pixel directly into the image buffer.

img->addr = start address of the image in memory
img->line_len = number of bytes for one full row
img->bpp / 8 = number of bytes for one pixel
x, y = pixel coordinates
So:

_pixel address = base address + y × bytes per row + x × bytes per pixel_

_pixel address = base address+y×bytes per row + x × bytes per pixel_

In code:

y * img->line_len moves to row y
x * (img->bpp / 8) moves to column x inside that row
dst becomes the exact memory location of pixel (x, y)
Then:

*(unsigned int *)dst = color;
means:

treat dst as a pointer to one pixel
store color there
So if bpp == 32, one pixel = 4 bytes, and color like 0xFFFFFF is written directly into those 4 bytes.

Equivalent idea:

char *base = img->addr;
char *pixel = base + y * img->line_len + x * 4;
*(unsigned int *)pixel = color;

Why this is needed:
mlx_get_data_addr() gives raw image memory, so drawing means manually writing the color at the correct offset.

If wanted, this can also be rewritten in a more explicit version with temporary variables for bytes_per_pixel and offset.


### screen_x and screen_y (draw_functions for the minimap)