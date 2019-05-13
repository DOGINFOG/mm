#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <dirent.h>

#include <Module.h>

#define VERSION "0.0.1.1"

std::vector<std::string> parse_args(const char *);

int exe(const std::vector<std::string> &);

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
#if 1
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
#else
		char *cmdline = new char[1024];
		std::cin.getline(cmdline, 1024);
		std::vector<std::string> args = parse_args(cmdline);
		delete[] cmdline;
#endif
		history.push_back(args);
		fprintf(stderr, "\n");
		if (exe(args) == 1)
			break;
		fprintf(stderr, "\n");
	}
	return 0;
}

int exe(const std::vector<std::string> &args) {
	if (!args.size() || args[0] == std::string("h") ||
		args[0] == std::string("?") || args[0] == std::string("help") ||
		args[0] == std::string("--help") || args[0] == std::string("-h") ||
		args[0] == std::string("--usage")) {
		fprintf(stderr, "Commands:\n"
						"\tl\t\tlist modules\n\n"
						"\td [id or name]\ttoggle disable module\n"
						"\tt [id or name]\ttoggle remove module\n"
						"\tm [id or name]\ttoggle skip mount module\n\n"
						"\th\t\tthis help message\n"
						"\tq\t\tquit\n");
		return 0;
	}
	if (args[0] == std::string("q") || args[0] == std::string("quit") ||
		args[0] == std::string("exit"))
		return 1;
	if (args[0] == std::string("d")) {
		Module mod;
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
		if (mod.name() == std::string("")) {
			fprintf(stderr, "Module %s not found\n", args[1].c_str());
			return 2;
		}

		mod.tog_enable();
		fprintf(stderr, "%s now is %s\n", mod.name().c_str(),
				mod.enabled() ? "enabled" : "disabled");
		return 0;
	}

	if (args[0] == std::string("r")) {
		Module mod;
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
		if (mod.name() == std::string("")) {
			fprintf(stderr, "Module %s not found\n", args[1].c_str());
			return 2;
		}

		mod.tog_remove();
		fprintf(stderr, "\"%s\" will%s remove\n", mod.name().c_str(),
				mod.will_remove() ? "" : " not");
		return 0;
	}

	if (args[0] == std::string("m")) {
		Module mod;
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
		if (mod.name() == std::string("")) {
			fprintf(stderr, "Module %s not found\n", args[1].c_str());
			return 2;
		}

		mod.tog_mount();
		fprintf(stderr, "\"%s\" mount is%s skipping\n", mod.name().c_str(),
				mod.skip_mount() ? "" : " not");
		return 0;
	}

	if (args[0] == std::string("z")) {
		for (auto &a : args)
			fprintf(stderr, "%s\n", a.c_str());
		return 0;
	}
	if (args[0] == std::string("l") || args[0] == std::string("ls") ||
		args[0] == std::string("list")) {
		fprintf(stderr, "list modules:\n");
		for (size_t i = 0; i < modules.size(); i++) {
			auto mod = modules[i];
			fprintf(stderr, "[%lu] [%c] %s\n", i, mod.tag(),
					mod.name().c_str());
		}
		return 0;
	}

	fprintf(stderr, "Unknown command: %s\n", args[0].c_str());
	return 2;
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
