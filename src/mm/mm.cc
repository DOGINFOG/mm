/** mm - MagiskManager CLI
Copyright (C) 2019 DOGINFOG <ftdabcde@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <map>
#include <functional>

#include <Module.h>

#define VERSION "0.0.1.2"

std::vector<std::string> parse_args(const char *);

int exe(const std::vector<std::string> &);

static int mm_list(Module &m, const std::vector<std::string> &args);
static int mm_disable(Module &m, const std::vector<std::string> &args);
static int mm_remove(Module &m, const std::vector<std::string> &args);
static int mm_mount(Module &m, const std::vector<std::string> &args);
static int mm_help(Module &m, const std::vector<std::string> &args);
static int mm_quit(Module &m, const std::vector<std::string> &args);

static std::map<std::string,
				std::function<int(Module &, const std::vector<std::string> &)>>
	mm_map{
		// clang-format off
		{"l", mm_list},
		{"ls", mm_list},
		{"list", mm_list},

		{"d", mm_disable},

		{"r", mm_remove},
		{"rm", mm_remove},

		{"m", mm_mount},
		{"mnt", mm_mount},

		{"h", mm_help},
		{"?", mm_help},
		{"help", mm_help},
		{"--help", mm_help},
		{"-h", mm_help},
		{"--usage", mm_help},

		{"q", mm_quit},
		{"quit", mm_quit},
		{"exit", mm_quit},
		// clang-format on
	};

void print_head() {
	static bool printed = false;
	if (printed)
		return;

	fprintf(stderr, "MagiskManager CLI v" VERSION "\n"
					"Copyright (C) 2019, DOGINFOG <ftdabcde@gmail.com>\n"
					"This program comes with ABSOLUTELY NO WARRANTY;\n"
					"This is free software, and you are welcome to\n"
					"redistribute it under certain conditions;\n\n");

	printed = true;
}

void print_magisk_not_installed() {
	fprintf(stderr, "Magisk not installed?\n"
					"This programm support Magisk 18+\n"
					"Install it, or update\n\n");
}

void print_help() {
	fprintf(stderr, "Commands:\n"
					"    l                 list modules\n\n"
					"    d [id or name]    toggle disable module\n"
					"    r [id or name]    toggle remove module\n"
					"    m [id or name]    toggle skip mount module\n\n"
					"    h                 this help message\n"
					"    q                 quit\n");
}

bool check_magisk() {
	DIR *dir = opendir(Module::mod_path.c_str());
	if (!dir) {
		return 1;
	}
	closedir(dir);
	return 0;
}

const std::vector<Module> modules{Module::getModuleList()};

int main(int argc, char **argv) {
	if (check_magisk()) {
		print_head();
		print_magisk_not_installed();
		return 1;
	}
	if (argc > 1) {
		size_t cmdline_len = 0;
		for (size_t i = 1; i < argc; i++)
			cmdline_len += strlen(argv[i]) + 1;
		char *cmdline = new char[cmdline_len];
		cmdline[0] = '\0';
		for (size_t i = 1; i < argc; i++) {
			strcat(cmdline, argv[i]);
			strcat(cmdline, " ");
		}
		std::vector<std::string> args = parse_args(cmdline);
		delete[] cmdline;
		return exe(args) > 1;
	}
	print_head();
	std::vector<std::vector<std::string>> history;
	for (;;) {
		static const char response[]{"\r?): "};
		fprintf(stderr, response);
		size_t history_index = 0;
		std::string cmdline;
		for (;;) {
			int tmp = getchar();
			if (tmp & 0xff00) {
				if (tmp != EOF) {
					continue;
				} else {
					break;
				}
			}
			if ((char)(tmp & 0xff) == 72) {
				history_index++;
				if (history_index > history.size()) {
					history_index--;
					continue;
				}
				fprintf(stderr, response);
				for (size_t i = 0; i < cmdline.length(); i++)
					fprintf(stderr, " ");
				fprintf(stderr, response);
				cmdline = std::string("");
				for (auto &arg : history[history.size() - history_index]) {
					std::cerr << " " << arg;
					cmdline += arg + std::string(" ");
				}
			}
			if (tmp == '\n' || tmp == '\r' || tmp == '\0')
				break;
			cmdline += (char)(tmp & 0xff);
		}
		std::vector<std::string> args = parse_args(cmdline.c_str());
		history.push_back(args);
		fprintf(stderr, "\n");
		if (exe(args) == 1)
			break;
		fprintf(stderr, "\n");
	}
	return 0;
}

int exe(const std::vector<std::string> &args) {
	Module mod;
	if (args.size() > 1) {
		bool is_digit = true;
		for (auto &c : args[1])
			if (c < '0' || c > '9') {
				is_digit = false;
				break;
			}
		if (is_digit)
			mod = modules[atol(args[1].c_str())];
		else
			for (auto &m : modules) {
				if (m.name() == args[1]) {
					mod = m;
				}
			}
	}
	auto func = mm_map.find(args[0]);
	if (func == mm_map.end()) {
		fprintf(stderr,
				"unknown command: %s\n"
				"enter '?' for help\n",
				args[0].c_str());
		return 2;
	}
	return func->second(mod, args);
}

std::vector<std::string> parse_args(const char *cmdline) {
	std::vector<std::string> o;
	for (const char *p = cmdline, *s = cmdline;; p++) {
		if (*p == ' ' || *p == '\n' || *p == '\0') {
			size_t size = p - s;
			if (size) {
				char *tmp = new char[size + 1];
				memcpy(tmp, s, size);
				tmp[size] = '\0';
				o.push_back(std::string(tmp));
				delete[] tmp;
			}
			s = p + 1;
		}
		if (*p == '\0')
			break;
	}
	return std::move(o);
}

int mm_list(Module &m, const std::vector<std::string> &args) {
	fprintf(stderr, "list modules:\n");
	for (size_t i = 0; i < modules.size(); i++) {
		auto mod = modules[i];
		fprintf(stderr, "[%lu] [%c] %s %s\n", i, mod.tag(), mod.name().c_str(),
				mod.updated() ? "(UPDATED)" : "");
	}
	return 0;
}

int mm_disable(Module &mod, const std::vector<std::string> &args) {
	if (mod.name() == std::string("")) {
		fprintf(stderr, "Module %s not found\n", args[1].c_str());
		return 2;
	}

	mod.tog_enable();
	fprintf(stderr, "%s now is %s\n", mod.name().c_str(),
			mod.disabled() ? "disabled" : "enabled");
	return 0;
}
int mm_remove(Module &mod, const std::vector<std::string> &args) {
	if (mod.name() == std::string("")) {
		fprintf(stderr, "Module %s not found\n", args[1].c_str());
		return 2;
	}

	mod.tog_remove();
	fprintf(stderr, "\"%s\" will%s remove\n", mod.name().c_str(),
			mod.will_remove() ? "" : " not");
	return 0;
}
int mm_mount(Module &mod, const std::vector<std::string> &args) {
	if (mod.name() == std::string("")) {
		fprintf(stderr, "Module %s not found\n", args[1].c_str());
		return 2;
	}

	mod.tog_mount();
	fprintf(stderr, "\"%s\" mount is%s skipping\n", mod.name().c_str(),
			mod.skip_mount() ? "" : " not");
	return 0;
}
int mm_help(Module &m, const std::vector<std::string> &args) {
	print_help();
	return 0;
}

int mm_quit(Module &m, const std::vector<std::string> &args) { return 1; }
