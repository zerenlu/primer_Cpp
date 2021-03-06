#include<algorithm>
#include<memory>
#include<iostream>
//this string is like a vector of chars
class String {
public:

	String(const char* a = nullptr);
	String(const String&);
	String& operator=(const String&);
	String(String&&) noexcept;
	String& operator=(String&&) noexcept;
	~String();

	const char* c_str() const { return elements; }
	size_t size() const { return end - elements; }
	size_t length() const { return end - elements - 1; }
private:
	char* elements;
	char* end;
	static std::allocator<char> alloc;

	std::pair<char*, char*> alloc_n_copy(const char*, const char*);

	void free();
};

std::allocator<char> String::alloc;

std::pair<char*, char*> String::alloc_n_copy(const char* begin, const char* end)
{
	auto str = alloc.allocate(end - begin);
	return { str, std::uninitialized_copy(begin, end, str) };

}

String::String(const char* a)
{
	const char* s1 = a;
	while (*s1) {
		++s1;
	}

	auto ptr_pair = alloc_n_copy(a, s1);
	elements = ptr_pair.first;
	end = ptr_pair.second;
}

String::String(const String& rhs)
{
	auto copy_ptr = alloc_n_copy(rhs.elements, rhs.end);
	elements = copy_ptr.first;
	end = copy_ptr.second;

}

void String::free()
{
	if (elements) {
		std::for_each(elements, end, [this](char& c) {alloc.destroy(&c); });
		alloc.deallocate(elements, end - elements);
	}
}

String::~String()
{
	free();
}

String& String::operator=(const String& rhs)
{
	auto newptr = alloc_n_copy(rhs.elements, rhs.end);
	free();
	elements = newptr.first;
	end = newptr.second;

	return *this;
}

String::String(String&&s) noexcept:
	elements(s.elements), end(s.end)
{
	s.elements = s.end = nullptr;
	std::cout << "String &&s" << std::endl;
}

String& String::operator=(String&&rhs) noexcept
{
	if (this != &rhs) {
		free();
		elements = rhs.elements;
		end = rhs.end;
		rhs.elements = rhs.end = nullptr;
	}

	std::cout << "operator String&&rhs" << std::endl;
	return *this;
}