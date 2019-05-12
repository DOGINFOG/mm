#pragma once
#ifndef MODULE_H

#include <string>
#include <vector>

class Module {
  private:
	std::string _name;
	static const std::string mod_path;

  public:
	Module();
	Module(const std::string &name);

	std::string name() const;
	bool enabled() const;
	bool will_delete() const;
	bool skip_mount() const;
	char tag() const;

	void tog_enable();
	void tog_mount();
	void tog_remove();

	static std::vector<Module> getModuleList();

	void operator=(const Module &m);
};

#endif
