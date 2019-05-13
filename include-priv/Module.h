#pragma once
#ifndef MODULE_H

#include <string>
#include <vector>

class Module {
  private:
	std::string _name;

	std::string full_path(const std::string &path = std::string("")) const;

  public:
	static const std::string mod_path;

	Module();
	Module(const std::string &name);

	std::string name() const;

	[[deprecated]] bool enabled() const;

	bool disabled() const;
	bool will_remove() const;
	bool skip_mount() const;
	char tag() const;

	void tog_enable();
	void tog_mount();
	void tog_remove();

	static std::vector<Module> getModuleList();

	void operator=(const Module &m);
};

#endif
