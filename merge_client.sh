#!/bin/bash
git fetch --all
git reset --hard upstream/master 

function git_merge() {
	echo "Mergin {$1}"
	git merge origin/"$1"
}

git_merge client_fng

# personal
if [[ -n ${1+x} && "${1}" == "1" ]]; then
	git_merge client_cmake
	#git_merge client_server_browser_ui
	git_merge client_misc
fi

git_merge client_quake
git_merge client_text_color_parser
git_merge client_modern_gl
git_merge client_better_mouse
git_merge pr_fng_scoreboard
#git_merge pr_fetch_two_masters

# currently broken git_merge client_console_upgrade
