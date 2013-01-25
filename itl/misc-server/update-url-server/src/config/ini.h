/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ini.h
 * @brief A manager class for ini files, for more information, please refer to:
 * 			<http://stlplus.sourceforge.net/stlplus/docs/ini_manager.html>
 * @author Internet
 * @date 2009-11-26
 */

#ifndef C_INI_H_
#define C_INI_H_

#include <vector>
#include <string>
#include <iostream>

/// Classes for managing INI (i.e. .ini) files
/// An INI file has the following format
///
///     file           ::= header { section }*
///     header         ::= { comment | blank }*
///     section        ::= section_header { declaration | comment | blank }*
///     section_header ::= '[' title ']' '\n'
///     declaration    ::= variable '=' value '\n'
///     comment        ::= ';' text '\n'
///     blank          ::= '\n'
///     title          ::= [~']']*
///     variable       ::= [~'=']*
///     value          ::= .*
///     text           ::= .*
///
/// Whitespace is trimmed from the leading and trailing ends of title, variable and value
/// Note: a header is represented internally as a Clint section (i.e. a section with no name)

class c_ini_body;

/// Ini-file manager class
class c_ini
{
public:
	c_ini(void);
	explicit c_ini(const std::vector<std::string>& filenames);
	c_ini(const c_ini&);
	c_ini& operator= (const c_ini&);
	~c_ini(void);



	/// add files starting with the most local file (e.g. the current project) which has depth 0
	/// and working back to the most global (e.g. the installation settings) which has a depth of size()-1
	/// This does nothing if the file has already been loaded - it is not permitted to manage the same file twice.
	/// Returns true if the file loaded okay or was already loaded (it is counted as successful if the file did
	/// not exist, only read errors cause a failure)
	bool add_file(const std::string& filename);

	/// as above, returns false if *none* of the files were added
	/// filenames[0] is the local file, and so on
	bool add_files(const std::vector<std::string>& filenames);

	/// saves modified ini files - returns true if all modified files were written successfully
	bool save(void);

	/// get the number of files being managed
	unsigned size(void) const;

	/// get the ini filename associated with a depth
	std::string filename(unsigned depth = 0) const;

	/// test whether a file in the ini manager is writable
	bool writable(unsigned depth = 0) const;

	/// test whether a file is empty
	/// An ini file is considered empty if it has no named sections and the header is empty or missing
	bool empty(unsigned depth = 0) const;

	/// erase the ini file from the ini manager and from the disk
	bool erase(unsigned depth = 0);

	/// remove the file from the ini manager but do not erase it from the disk
	bool remove(unsigned depth = 0);



	/// returns the union of all section names in all of the ini files
	std::vector<std::string> section_names(void) const;

	/// returns the section names in one of the ini files
	std::vector<std::string> section_names(unsigned depth) const;

	/// tests whether a section is found in any of the ini files
	bool section_exists(const std::string& title) const;

	/// tests whether the section is found in the specific ini file
	bool section_exists(const std::string& title, unsigned depth) const;

	/// adds a section to the specified ini file - does nothing if it is already present
	bool add_section(const std::string& section, unsigned depth = 0);

	/// test whether a section is empty
	bool empty_section(const std::string& section, unsigned depth = 0);

	/// removes a section from the specified ini file if it exists there but cannot remove it from any other file
	bool erase_section(const std::string& section, unsigned depth = 0);

	/// removes all the contents of a section from the specified ini file but keeps the empty section
	bool clear_section(const std::string& section, unsigned depth = 0);



	/// test whether a variable exists in any of the ini files
	bool variable_exists(const std::string& section, const std::string variable) const;

	/// test whether a variable exists in specified ini file
	bool variable_exists(const std::string& section, const std::string variable, unsigned depth) const;

	/// get the union of all variables declared in all ini files
	std::vector<std::string> variable_names(const std::string& section) const;

	/// get the set of all varaibale names from one file
	std::vector<std::string> variable_names(const std::string& section, unsigned depth) const;

	/// get the depth of the first ini file to define a variable
	/// returns 0 if defined in the local ini file, etc. Returns (unsigned)-1 if the variable doesn't exist
	unsigned variable_depth(const std::string& section, const std::string variable) const;

	/// get the filename that first defines the variable
	std::string variable_filename(const std::string& section, const std::string variable) const;
	/// ditto for its linenumber within that file
	unsigned variable_linenumber(const std::string& section, const std::string variable) const;

	/// get the value of a variable as a single unprocessed string
	/// if the variable does not exist the string will be empty, but beware that
	/// you also get an empty string if a variable exists but has no value
	/// you can differentiate between the two cases by using variable_exists_all above
	std::string variable_value(const std::string& section, const std::string variable) const;

	/// get the value from the specified file
	std::string variable_value(const std::string& section, const std::string variable, unsigned depth) const;

	/// get the value of a variable as a processed string
	/// processing splits the value at commas and furthermore supports quoted strings (so that values can contain commas for example)
	/// quoted strings are dequoted before they are added to the result
	/// the result is a vector of dequoted strings, one per value in the comma-separated list
	std::vector<std::string> variable_values(const std::string& section, const std::string variable) const;

	/// get the processed variable from the specified file
	std::vector<std::string> variable_values(const std::string& section, const std::string variable, unsigned depth) const;

	/// add a variable to the specified file
	bool add_variable(const std::string& section, const std::string& variable, const std::string& value, unsigned depth = 0);

	/// add a variable as a processed string
	/// processing means that the values in the string vector are converted into a comma-separated list
	/// values containing reserved characters are automatically quoted - so you should not even try to quote them yourself
	bool add_variable(const std::string& section, const std::string& variable, const std::vector<std::string>& values, unsigned depth = 0);

	/// erase a variable from the specified file
	/// this does not remove the variable from other ini files, so the variable may still exist
	/// to mask a global variable, set the variable to an empty string instead
	bool erase_variable(const std::string& section, const std::string& variable, unsigned depth = 0);



	/// add a comment to the specified ini file
	bool add_comment(const std::string& section, const std::string& comment, unsigned depth = 0);

	/// add a blank line to the specified ini file
	bool add_blank(const std::string& section, unsigned depth = 0);

	bool print(std::ostream&) const;

private:
	friend class c_ini_body;
	c_ini_body* m_body;
};

/// diagnostic print routine
std::ostream& operator << (std::ostream&, const c_ini&);

#endif /* C_INI_H_ */
