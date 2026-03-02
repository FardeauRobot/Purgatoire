#!/usr/bin/env bash
# start.sh - Open a terminal with 3 tabs: Spotizer, Spotify Web, and Intra
# Usage: start.sh

# Tab 1: cd into Spotizer repo and launch the visualizer
# Tab 2: Open Spotify on web
# Tab 3: Open 42 Intra

# Open first tab in a new window
gnome-terminal --title="Spotizer" -- zsh -ic "Spotizer && Spot; exec zsh" &
sleep 1

# Append remaining tabs to the existing window
gnome-terminal --tab --title="Spotify" -- zsh -ic "PlaySpot; exec zsh"
sleep 0.5
gnome-terminal --tab --title="Intra"   -- zsh -ic "Intra; exec zsh"
