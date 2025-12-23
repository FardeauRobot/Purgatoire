# Notes

- If the window fails to open, ensure X11 is available (e.g., not over bare SSH without forwarding).
- If linking fails for `libmlx`, verify you have X11 dev packages installed and rebuild `includes/minilibx-linux`.
- If maps are rejected, run with a small test map and check for rectangular shape and surrounding walls.
- Use `make fclean && make` after touching library headers to avoid stale objects.
