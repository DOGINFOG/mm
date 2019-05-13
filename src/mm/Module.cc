#include <fstream>
#include <dirent.h>

#include <Module.h>

const std::string Module::mod_path(
#if !defined(TEMP) && 0
	"/data/adb/modules/"
#else
#warning this is temporary directory
	"./test_env/"
#endif
);

const static std::string disable_path("/disable");
const static std::string remove_path("/remove");
const static std::string skip_mount_path("/skip_mount");

Module::Module(const std::string &name) : _name(name) {}
Module::Module() : _name(std::string("")) {}

std::string Module::name() const { return _name; }

bool Module::enabled() const { return !disabled(); }

bool Module::disabled() const {
	std::fstream fin(mod_path + _name + disable_path, std::ios::in);
	if (!fin.is_open())
		return false;
	fin.close();
	return true;
}
bool Module::will_remove() const {
	std::fstream fin(mod_path + _name + remove_path, std::ios::in);
	if (!fin.is_open())
		return false;
	fin.close();
	return true;
}
bool Module::skip_mount() const {
	std::fstream fin(mod_path + _name + skip_mount_path, std::ios::in);
	if (!fin.is_open())
		return false;
	fin.close();
	return true;
}

char Module::tag() const {
	if (will_remove())
		return 'X';
	if (disabled())
		return ' ';
	if (skip_mount())
		return 'M';
	return '*';
}

void Module::tog_enable() {
	if (disabled()) {
		remove(full_path(disable_path).c_str());
		return;
	}
	std::fstream fout(full_path(disable_path), std::ios::out);
	fout.close();
}

void Module::tog_remove() {
	if (will_remove()) {
		remove(full_path(remove_path).c_str());
		return;
	}
	std::fstream fout(full_path(remove_path), std::ios::out);
	fout.close();
}

void Module::tog_mount() {
	if (skip_mount()) {
		remove(full_path(skip_mount_path).c_str());
		return;
	}
	std::fstream fout(full_path(skip_mount_path), std::ios::out);
	fout.close();
}

std::vector<Module> Module::getModuleList() {
	std::vector<Module> o;
	DIR *dir = opendir(mod_path.c_str());
	struct dirent *ent;
	if (dir) {
		while ((ent = readdir(dir))) {
			std::string name(ent->d_name);
			if (name != std::string(".") && name != std::string("..") &&
				name != std::string("lost+found") &&
				name != std::string(".core"))
				o.push_back(Module(std::move(name)));
			// free(ent);
		}
		closedir(dir);
	}
	return std::move(o);
}

void Module::operator=(const Module &m) { _name = m._name; }

std::string Module::full_path(const std::string &path) const {
	return mod_path + _name + std::string(path[0] == '/' ? "" : "/") + path;
}
