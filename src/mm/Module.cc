#include <fstream>
#include <dirent.h>

#include <Module.h>

#warning this is temporary directory
const std::string Module::mod_path("./_magisk/");

Module::Module(const std::string &name) : _name(name) {}
Module::Module() : _name(std::string("")) {}

std::string Module::name() const { return _name; }

bool Module::enabled() const {
	std::fstream fin(mod_path + _name + std::string("/disable"), std::ios::in);
	if (!fin.is_open())
		return true;
	fin.close();
	return false;
}
bool Module::will_remove() const {
	std::fstream fin(mod_path + _name + std::string("/remove"), std::ios::in);
	if (!fin.is_open())
		return false;
	fin.close();
	return true;
}
bool Module::skip_mount() const {
	std::fstream fin(mod_path + _name + std::string("/skip_mount"),
					 std::ios::in);
	if (!fin.is_open())
		return false;
	fin.close();
	return true;
}

char Module::tag() const {
	if (will_remove())
		return 'X';
	if (!enabled())
		return ' ';
	if (skip_mount())
		return 'M';
	return '*';
}

void Module::tog_enable() {
	if (!enabled()) {
		remove((mod_path + _name + std::string("/disable")).c_str());
		return;
	}
	std::fstream fout(mod_path + _name + std::string("/disable"),
					  std::ios::out);
	fout.close();
}

void Module::tog_remove() {
	if (will_remove()) {
		remove((mod_path + _name + std::string("/remove")).c_str());
		return;
	}
	std::fstream fout(mod_path + _name + std::string("/remove"), std::ios::out);
	fout.close();
}

void Module::tog_mount() {
	if (skip_mount()) {
		remove((mod_path + _name + std::string("/skip_mount")).c_str());
		return;
	}
	std::fstream fout(mod_path + _name + std::string("/skip_mount"),
					  std::ios::out);
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
				name != std::string("lost+found"))
				o.push_back(Module(std::move(name)));
			// free(ent);
		}
	}
	return std::move(o);
}

void Module::operator=(const Module &m) { _name = m._name; }
