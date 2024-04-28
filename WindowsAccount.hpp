#pragma once
#include "Roles.hpp"
#include <string>
#include <odb/core.hxx>
using std::string;

#pragma db object
class Windows_Account {
public:
	Windows_Account(string name, Roles* access_level) : 
		name_(name), access_level_(access_level) {}

	// Getters

	/// <summary>
	/// The unique identifier for this account.
	/// </summary>
	/// <returns></returns>
	const int Id();
	/// <summary>
	/// The name of the account.
	/// </summary>
	/// <returns></returns>
	const string Name();
	/// <summary>
	/// The access level of the account.
	/// </summary>
	/// <returns></returns>
	const Roles* AccessLevel();

	// Setters

	/// <summary>
	/// Sets the name of the account.
	/// </summary>
	/// <param name="name"></param>
	void SetName(string name);
	/// <summary>
	/// Sets the access level of the account.
	/// </summary>
	/// <param name="access_level"></param>
	void SetAccessLevel(Roles* access_level);

private:
	friend class odb::access;
	Windows_Account() {}

	// Persistent fields
	#pragma db id auto
	int _id;
	string name_;
	Roles* access_level_;
};