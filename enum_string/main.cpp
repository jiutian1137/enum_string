//--------------------------------------------------------------------------------------
// Copyright (c) 2020 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#include <array>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <iostream>
#include <algorithm>


template<typename _Ty1, typename _Ty2>
std::ostream& operator<<(std::ostream& _Ostr, const std::vector<std::pair<_Ty1, _Ty2>>& _Source) {
	for (const auto& _Pair : _Source) {
		_Ostr << "first: " << _Pair.first << "\tsecond: " << _Pair.second << std::endl;
	}
	return _Ostr;
}

enum enum_struct_mapping_types {
	switch_type,
	if_n_type
};

struct enum_struct : public std::vector<std::pair<std::string, std::string>> {
	using _Mybase = std::vector<std::pair<std::string, std::string>>;
	using value_type = std::pair<std::string, std::string>;

	std::string zero_item_str() const {
		return  (_Ref_zero_item == nullptr ? "" : _Ref_zero_item->first);
	}

	std::string _Name;
	value_type* _Ref_zero_item = nullptr;
	bool _Mapping_type = switch_type;
};

std::string enum_mapping_function_switch(const enum_struct& _Enums) {
	if (_Enums.empty()) {
		return std::string();
	}

	std::array<char, 256> _Tmp;
	snprintf(_Tmp.data(), _Tmp.size() - 1, "enum_string(%s _Flags){\n"
		"switch(_Flags){\n", _Enums._Name.c_str());
	std::string _Str = _Tmp.data();

	for (const auto& _Enum_item : _Enums) {
		_Tmp.fill('\0');
		snprintf(_Tmp.data(), _Tmp.size()-1, 
			"case %s:\n"
			"\tstatic_cast<_Mybase&>(*this) = \"%s\"; \n"
			"\tbreak;\n", _Enum_item.first.c_str(), _Enum_item.first.c_str());
		_Str += _Tmp.data();
	}
	_Tmp.fill('\0');
	snprintf(_Tmp.data(), _Tmp.size() - 1, 
		"default: \n"
		"\tthrow std::exception(\"[Unkown enum-item] ->[enum_string(%s)]\"); \n", _Enums._Name.c_str());
	_Str += _Tmp.data();

	_Str += "\t};\n"
			"}";
	return _Str;
}

std::string enum_mapping_function_ifn(const enum_struct& _Enums) {
	if (_Enums.empty()) {
		return std::string();
	}

	std::array<char, 512> _Tmp;
	snprintf(_Tmp.data(), _Tmp.size() - 1, "enum_string(%s _Flags){\n", _Enums._Name.c_str());
	std::string _Str = _Tmp.data();

	for (const auto& _Enum_item : _Enums) {
		_Tmp.fill('\0');
		snprintf(_Tmp.data(), _Tmp.size()-1, 
			"if(_Flags & %s)\n"
			"\tstatic_cast<_Mybase&>(*this) += \"%s|\"; \n", _Enum_item.first.c_str(), _Enum_item.first.c_str());
		_Str += _Tmp.data();
	}

	_Tmp.fill('\0');
	snprintf(_Tmp.data(), _Tmp.size() - 1, 
	"if(_Mybase::empty()){\n"
	"\tif(_Flags == 0){\n"
	"\t\tstatic_cast<_Mybase&>(*this) = \"%s\";\n"
	"\t}\n"
	"\tif(_Mybase::empty()){\n"
	"\t\tthrow std::exception(\"[Unkown enum-item] ->[enum_string(%s)]\");\n"
	"\t}\n"
	"} else if(_Mybase::back() == '|'){\n"
	"\t_Mybase::pop_back();\n"
	"}", _Enums.zero_item_str().c_str(), _Enums._Name.c_str());
	_Str += _Tmp.data();

	_Str += "}";
	return _Str;
}


int main(int argc, char** argv) {
	enum CommandLineType {
		_fin  = 0b00000001,
		_fout = 0b00000010,
		_asw  = 0b00000100,
		_aif  = 0b00001000
	};
	std::string _Input_filename = "input.txt";
	std::string _Output_filename = "output.txt";
	bool _Is_all_switch = false;
	bool _Is_all_ifn    = false;

	for (int i = 1; i != argc; ++i) {
		//std::cout << argv[i] << std::endl;
		if (std::string(argv[i]) == "-fin") {
			++i;
			if (i != argc) {
				_Input_filename = argv[i];
			}
		} else if (std::string(argv[i]) == "-fout") {
			++i;
			if (i != argc) {
				_Output_filename = argv[i];
			}
		} else if (std::string(argv[i]) == "-asw") {
			_Is_all_switch = true;
		} else if (std::string(argv[i]) == "-aif") {
			_Is_all_ifn = true;
		} else if (std::string(argv[i]) == "-h") {
			std::cout << "-fin \"filename\" \n"
					  << "-fout \"filename\" \n"
					  << "-asw \n"
					  << "-aif \n";
			return 0;
		}
	}
	//std::cin.get();

	if (_Is_all_switch && _Is_all_ifn) {
		std::cout << "-asw not or -aif" << std::endl;
		return 0;
	}

	std::string _Test_data = 
        "enum D3D12_BLEND\n"
		"{ \n"
		"D3D12_BLEND_ZERO = 1,\n"
		"D3D12_BLEND_ONE = 2,\n"
		"D3D12_BLEND_SRC_COLOR = 3,\n"
		"D3D12_BLEND_INV_SRC_COLOR = 4,\n"
		"D3D12_BLEND_SRC_ALPHA = 5,\n"
		"D3D12_BLEND_INV_SRC_ALPHA = 6,\n"
		"D3D12_BLEND_DEST_ALPHA = 7,\n"
		"D3D12_BLEND_INV_DEST_ALPHA = 8,\n"
		"D3D12_BLEND_DEST_COLOR = 9,\n"
		"D3D12_BLEND_INV_DEST_COLOR = 10,\n"
		"D3D12_BLEND_SRC_ALPHA_SAT = 11,\n"
		"D3D12_BLEND_BLEND_FACTOR = 14,\n"
		"D3D12_BLEND_INV_BLEND_FACTOR = 15,\n"
		"D3D12_BLEND_SRC1_COLOR = 16,\n"
		"D3D12_BLEND_INV_SRC1_COLOR = 17,\n"
		"D3D12_BLEND_INV_SRC1_COLOR = 17,\n"
		"D3D12_BLEND_INV_SRC1_COLOR = 17,\n"
		"D3D12_BLEND_INV_SRC1_COLOR = 17,\n"
		//"D3D12_BLEND_INV_SRC1_ALPHA"
		"} 	D3D12_BLEND; \n";
	//"C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/um/d3d12.h"	
	std::ifstream _Fin;
	_Fin.open(_Input_filename);
	_Fin.seekg(0, _Fin.end);
	auto _Content = std::string(static_cast<size_t>(_Fin.tellg()), ' ');
	_Fin.seekg(0, _Fin.beg);
	_Fin.read(&_Content[0], _Content.size());
	_Fin.close();
		
	std::string _Match_varname    = "([A-Za-z0-9_]+)";
	std::string _Match_assign_expres = "(" "[=]" "([ \t\n]*)" "((\.|[\n])+)" ")";
	//                                 =             var_name
	std::string _Match_enum_begin = "(" "enum([ \t\n]+)" + _Match_varname + "([ \t\n]*)" "[{]" ")";
	//								     enum                  var_name                    {
	std::string _Match_enum_item1 = "(" "([ \t\n]*)" + _Match_varname + "([ \t\n]*)" + ")";
	//						           item_name                   [,}]
	std::string _Match_enum_item2 = "(" "([ \t\n]*)" + _Match_varname + "([ \t\n]*)" + _Match_assign_expres + ")";
	//                                       item_name                      =  var_expres [,}]
	std::string _Match_enum_end   = "(([ \t\n]*)[}])";

	std::regex _Rex = std::regex("[A-Za-z_]((\.|[\n])+)" "[,}]");
	std::regex _Rex_item1 = std::regex(_Match_enum_item1);
	std::regex _Rex_item2 = std::regex(_Match_enum_item2);
	std::regex _Rex_varname = std::regex(_Match_varname);


	// 1. match
	const std::string _Keyword_enum = "enum ";
	bool _Found_enum = false;
	bool _In_struct  = false;

	std::vector<enum_struct> _Enums;
	auto       _First = _Content.begin();
	const auto _Last  = _Content.end();
	auto       _Where = _First;
	for (; _First != _Last; ++_First) {
		if (_Found_enum) {
			if (_In_struct) {
				_Where = std::find(_First, _Last, '}');
				if (_Where != _Last) {
					// regex [_First, _Where]
					++_Where;

					const auto _Is_end = [](char _Ch) { return _Ch == ',' || _Ch == '}'; };
					auto _Sfirst = _First;
					auto _Slast  = std::find_if(_Sfirst, _Where, _Is_end);
					while (_Slast != _Where) {
						if (std::regex_match(_Sfirst, _Slast, _Rex_item1)) {// D3D_TYPE
							_Enums.back().push_back( { std::string(_Sfirst, _Slast), std::string() } );
						} else if(std::regex_match(_Sfirst, _Slast, _Rex_item2)){// D3D_TYPE = 0x0010
							auto _Mid1 = std::find(_Sfirst, _Slast, '=');
							auto _Mid2 = _Mid1;
							do
							{
								--_Mid1;
							} while (isspace(*_Mid1));
							++_Mid1;

							do
							{
								++_Mid2;
							} while (isspace(*_Mid2));
							_Enums.back().push_back({ std::string(_Sfirst, _Mid1), std::string(_Mid2, _Slast) });
						}

						_Sfirst = _Slast + 1;
						_Slast  = std::find_if(_Sfirst, _Where, _Is_end);
					}
				}
				_First      = _Where;
				_Found_enum = false;
				_In_struct  = false;
			} else {// found enum-name
				_Where = std::find(_First, _Last, '{');
				if (_Where != _Last) {
					_Enums.back()._Name.assign(_First, _Where);
					_In_struct = true;
				}
				_First = _Where;
			}
		} else if (*_First == 'e' && std::equal(_Keyword_enum.begin(), _Keyword_enum.end(), _First)) {
			_Enums.push_back(enum_struct());
			_Found_enum = true;
			std::advance(_First, _Keyword_enum.size() - 1);
		}
	}

	// 2.correct
	for (auto& _Em : _Enums) {
		auto& _Name = _Em._Name;
		if (!_Name.empty()) {
			auto _Last = std::remove_copy_if(_Name.begin(), _Name.end(), _Name.begin(), isspace);
			_Name.erase(_Last, _Name.end());
		}

		for (auto& _Item : _Em) {
			auto& _Key = _Item.first;
			if (!_Key.empty()) {
				auto _Last = std::remove_copy_if(_Key.begin(), _Key.end(), _Key.begin(), isspace);
				_Key.erase(_Last, _Key.end());
			}

			auto& _Val = _Item.second;
			if (!_Val.empty()) {
				auto _Last = std::remove_copy_if(_Val.begin(), _Val.end(), _Val.begin(), isspace);
				_Val.erase(_Last, _Val.end());
			}

			if (!_Val.empty()) {
				int _Base = 10;
				if (_Val.size() > 2 && _Val[0] == '0') {
					if (_Val[1] == 'x' || _Val[1] == 'X') {// 0x | 0X
						_Base = 16;
						_Em._Mapping_type = if_n_type;
					} else if (_Val[1] == 'b' || _Val[1] == 'B') {// 0b | 0B
						_Base = 2;
						_Em._Mapping_type = if_n_type;
					}
				}
		
				try {
					if (std::stoll(_Val, nullptr, _Base) == 0) {
						_Em._Ref_zero_item = &_Item;
					}
				} catch (const std::exception&) {
					_Val.clear();
				}
			}
		}
	}

	// 3. show
	/*for (const auto& _Item : _Enums) {
		std::cout << _Item._Name << std::endl;
		std::cout << _Item << std::endl;
		std::cout << std::endl;
	}*/

	// 3. optimize
	if (_Is_all_switch) {
		for (auto& _Em : _Enums) {
			_Em._Mapping_type = switch_type;
		}
	} else if (_Is_all_switch) {
		for (auto& _Em : _Enums) {
			_Em._Mapping_type = if_n_type;
		}
	}

	// 4. output
	std::ofstream _Fout;
	_Fout.open(_Output_filename, std::ios::out);
	std::ostream& _Ostr = _Fout.is_open() ? _Fout : std::cout;
	for (const auto& _Em : _Enums) {
		if (_Em._Mapping_type == if_n_type) {
			_Ostr << enum_mapping_function_ifn(_Em) << std::endl;
		} else {
			_Ostr << enum_mapping_function_switch(_Em) << std::endl;
		}

		_Ostr << std::endl;
	}
	_Fout.close();

	return 0;
}