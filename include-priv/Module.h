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

	bool updated() const;

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
