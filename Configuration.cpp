#include "Globals.h"
#include "Configuration.h"

ConfigurationValue& ConfigurationValue::operator=(const wchar_t* value)
{
	mValue = value;

	WritePrivateProfileString(mParent.mSection,mName,mValue,mParent.mParent.mName);

	return *this;
}

ConfigurationValue::operator const wchar_t*(void)
{
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,mValue,buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue;
}

const wchar_t* ConfigurationValue::operator()(const wchar_t* defaul)
{
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,defaul,buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue;
}

ConfigurationValue& ConfigurationValue::operator=(long value)
{
	mValue.FromInt(value);
	WritePrivateProfileString(mParent.mSection,mName,mValue,mParent.mParent.mName);

	return *this;
}

ConfigurationValue::operator long(void)
{
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,mValue,buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue.AsInt();
}

long ConfigurationValue::operator()(long defaul)
{
	WCHAR def[64];
	_ltow(defaul,def,10);
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,def,buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue.AsInt();
}

ConfigurationValue& ConfigurationValue::operator=(bool value)
{
	mValue.FromBool(value);
	WritePrivateProfileString(mParent.mSection,mName,mValue,mParent.mParent.mName);

	return *this;
}

ConfigurationValue::operator bool(void)
{
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,mValue,buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue.AsBool();
}

bool ConfigurationValue::operator()(bool defaul)
{
	WCHAR buffer[512];
	if(GetPrivateProfileString(mParent.mSection,mName,defaul ? L"True" : L"False",buffer,_countof(buffer),mParent.mParent.mName))
		mValue = buffer;

	return mValue.AsBool();
}