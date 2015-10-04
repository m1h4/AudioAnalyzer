#pragma once

class ConfigurationValue
{
public:
	ConfigurationValue(class ConfigurationSection& parent,const wchar_t* name) : mParent(parent), mName(name) {}
	ConfigurationValue(const ConfigurationValue& other) : mParent(other.mParent), mName(other.mName) {}
	~ConfigurationValue(void) {}

	//ConfigurationValue& operator=(const ConfigurationValue& other) { m_Parent = other.m_Parent; m_Name = other.m_Name; }

public:
	// String
	ConfigurationValue& operator=(const wchar_t* value);
	operator const wchar_t*(void);
	const wchar_t* operator()(const wchar_t* defaul);

	// Integer
	ConfigurationValue& operator=(long value);
	operator long(void);
	long operator()(long defaul);

	// Boolean
	ConfigurationValue& operator=(bool value);
	operator bool(void);
	bool operator()(bool defaul);

protected:
	class ConfigurationSection& mParent;
	String mName;
	String mValue;
};

class ConfigurationSection
{
public:
	ConfigurationSection(class Configuration& parent, const wchar_t* name) : mParent(parent), mSection(name) {}
	ConfigurationSection(const ConfigurationSection& other) : mParent(other.mParent), mSection(other.mSection) {}
	~ConfigurationSection(void) {}

	//ConfigurationSection& operator=(const ConfigurationSection& other) { m_Parent = other.m_Parent; m_Section = other.m_Section; }

public:
	ConfigurationValue operator[](const wchar_t* value) { return ConfigurationValue(*this, value); }

protected:
	friend ConfigurationValue;

	class Configuration&	mParent;
	String					mSection;
};

class Configuration
{
public:
	Configuration(const wchar_t* name) : mName(name) {}
	Configuration(const Configuration& other) : mName(other.mName) {}
	~Configuration(void) {}

	//Configuration& operator=(const Configuration& other) { m_Name = other.m_Name; }

public:
	ConfigurationSection operator[](const wchar_t* section) { return ConfigurationSection(*this, section); }

protected:
	friend ConfigurationSection;
	friend ConfigurationValue;

	String		mName;
};