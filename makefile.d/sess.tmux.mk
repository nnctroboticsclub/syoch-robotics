tms:
	tmux start

ct: tms
	tmux set-option -g default-terminal screen-256color
	tmux set -g terminal-overrides 'xterm:colors=256'

	tmux set-option -g status-position top
	tmux set-option -g status-left-length 90
	tmux set-option -g status-right-length 90

	tmux set-option -g status-left "#S/#I:#W.#P"
	tmux set-option -g status-right ''
	tmux set-option -g status-justify centre
	tmux set-option -g status-bg "colour238"
	tmux set-option -g status-fg "colour255"

	tmux bind '|' split-window -h
	tmux bind '-' split-window -v

	tmux set-option -g mouse on
	tmux bind -n WheelUpPane if-shell -F -t = "#{mouse_any_flag}" "send-keys -M" "if -Ft= '#{pane_in_mode}' 'send-keys -M' 'copy-mode -e'"

it:
	tmux new-session -d -s $(TMUX_SESSION) -n work

	tmux new-window -t $(TMUX_SESSION):1 -n log
	tmux split-window -t $(TMUX_SESSION):log.0 -v
	tmux split-window -t $(TMUX_SESSION):log.0 -h
	tmux split-window -t $(TMUX_SESSION):log.2 -h
	tmux split-window -t $(TMUX_SESSION):log.3 -h
	tmux resize-pane -t $(TMUX_SESSION):log.1 -x 20

	tmux select-window -t $(TMUX_SESSION):work
	tmux select-window -t $(TMUX_SESSION):log
	tmux send-keys -t $(TMUX_SESSION):log.0 'make me' C-m
	tmux send-keys -t $(TMUX_SESSION):log.1 'make ws' C-m
	tmux send-keys -t $(TMUX_SESSION):log.2 'make ms1' C-m
	tmux send-keys -t $(TMUX_SESSION):log.3 'make ms2' C-m
	tmux send-keys -t $(TMUX_SESSION):log.4 'cd ws-relay; python3 main.py' C-m

at:
	tmux attach -t $(TMUX_SESSION)

stm: ct it at
stw:
	tmux new-session -d -s $(TMUX_SESSION)-work -n work
	tmux attach -t $(TMUX_SESSION)-work