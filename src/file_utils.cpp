// Copyright (c) 2018-2022  Robert J. Hijmans
//
// This file is part of the "spat" library.
//
// spat is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// spat is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with spat. If not, see <http://www.gnu.org/licenses/>.

#include "spatBase.h"
#include <fstream>
#include <random>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>

bool write_text(std::string filename, std::vector<std::string> s) {
	std::ofstream f;
	f.open(filename);
	if (f.is_open()) {
		for (size_t i=0; i<s.size(); i++) {
			f << s[i] << std::endl;
		}
		f.close();
		return true;
	} else {
		return false;
	}
}


std::vector<std::string> read_text(std::string filename) {
	std::vector<std::string> s;
	std::string line;
	std::ifstream f(filename);
	if (f.is_open())  {
		while (getline(f, line)) {
			if (line.empty()) {
				s.push_back("");
			} else {
				s.push_back(line);
			}
		}
		f.close();
	}
	return s;
}


std::string getFileExt(const std::string& s) {
	size_t i = s.rfind('.', s.length());
	if (i != std::string::npos) {
		return(s.substr(i, s.length() - i));
	}
	return("");
}

std::string setFileExt(const std::string& s, const std::string& ext) {
	size_t i = s.rfind('.', s.length());
	if (i != std::string::npos) {
		return(s.substr(0, i) + ext);
	}
	return(s + ext);
}

std::string noext(std::string filename) {
	const size_t p = filename.rfind('.');
	if (std::string::npos != p) {
		filename.erase(p);
	}
	return filename;
}

std::string basename(std::string filename) {
	const size_t i = filename.find_last_of("\\/");
	if (std::string::npos != i) {
		filename.erase(0, i + 1);
	}
	return filename;
}


std::string basename_noext(std::string filename) {
	filename = basename(filename);
	filename = noext(filename);
	return filename;
}


std::string dirname(std::string filename) {
	const size_t i = filename.find_last_of("\\/");
	if (std::string::npos != i) {
		return( filename.substr(0, i) );
	} else {
		return ("");
	}
}

bool file_exists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}


bool path_exists(std::string path) {
	struct stat info;
	stat(path.c_str(), &info );
	if(info.st_mode & S_IFDIR) {
		return true;
	}
	return false;
}



bool canWrite(std::string filename) {
	FILE *fp = fopen(filename.c_str(), "w");
	if (fp == NULL) {
		return false;
	}
	fclose(fp);
	remove(filename.c_str());
	return true;
}


std::string get_path(const std::string filename) {
	size_t found = filename.find_last_of("/\\");
	std::string result = filename.substr(0, found);
	return result;
}

bool filepath_exists(const std::string& name) {
	std::string p = get_path(name);
	return path_exists(p);
}



bool can_write(std::string filename, bool overwrite, std::string &msg) {
	if (file_exists(filename)) {
		if (overwrite) {
			if (remove(filename.c_str()) != 0) {
				msg = ("cannot overwrite existing file");
				return false;
			}
			//std::string aux = filename + ".aux.xml";
			//remove(aux.c_str());
			std::vector<std::string> exts = {".vat.dbf", ".vat.cpg", ".json"};
			for (size_t i=0; i<exts.size(); i++) {
				std::string f = filename + exts[i];
				if (file_exists(f)) {
					remove(f.c_str());
				}
			}
		} else {
			msg = "file exists";
			return false;
		}
	} else if (!canWrite(filename)) {
		std::string path = get_path(filename);
		if (!path_exists(path)) {
			msg = "path does not exist";
		} else {
			msg = "cannot write file";
		}
		return false;
	}
	return true;
}



std::string tempFile(std::string tmpdir, unsigned pid, std::string ext) {
    std::vector<char> characters = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K',
    'L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z' };
    std::default_random_engine generator(std::random_device{}());
	double seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
    std::uniform_int_distribution<> distrib(0, characters.size()-1);
    auto draw = [ characters, &distrib, &generator ]() {
		return characters[ distrib(generator) ];
	};
    std::string filename(15, 0);
    std::generate_n(filename.begin(), 15, draw);
	filename = tmpdir + "/spat_" + filename + "_" + std::to_string(pid) + ext;
	return filename;
}
