#!/bin/bash
#

tmux new-session \; \
	send-keys 'glances' C-m \; \
	split-window -v \; \
	send-keys 'nvtop' C-m \;
