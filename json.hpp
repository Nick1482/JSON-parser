#include <iostream>
#include <string>
#include <limits>

struct json_exception {
	std::string msg;
};

class json {

public:
	
	struct list_iterator;
	struct dict_iterator;
	struct const_list_iterator;
	struct const_dict_iterator;
	
	json();
	json(json const&);
	json(json&&);
	~json();
	
	json& operator=(json const&);
	json& operator=(json&&);

	bool is_list() const;
	bool is_dictionary() const;
	bool is_string() const;
	bool is_number() const;
	bool is_bool() const;
	bool is_null() const;

	json const& operator[](std::string const&) const;
	json& operator[](std::string const&);

	list_iterator begin_list();
	const_list_iterator begin_list() const;
	list_iterator end_list();
	const_list_iterator end_list() const;

	dict_iterator begin_dict();
	const_dict_iterator begin_dict() const;
	dict_iterator end_dict();
	const_dict_iterator end_dict() const;

	double& get_number();
	double const& get_number() const;

	bool& get_bool();
	bool const& get_bool() const;

	std::string& get_string();
	std::string const& get_string() const;

	void set_string(std::string const&);
	void set_bool(bool);
	void set_number(double);
	void set_null();
	void set_list();
	void set_dictionary();
	void push_front(json const&);
	void insert(std::pair<std::string,json> const&);

private:
		
	struct impl;
	impl* pimpl;
	
};

std::ostream& operator<<(std::ostream& lhs, json const& rhs);
std::istream& operator>>(std::istream& lhs, json& rhs);
