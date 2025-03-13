#include <iostream>
#include <vector>
#include <string>
#include <assert.h>

class Cat { // ����� "���"
	int age; // �������
	std::string name; // ������ ����
	std::string owner; // ��� ���������
	bool gender; // ��� ����
	double tail_length; // ����� ������
	bool deleted = false; // ����, �������� �� ������ ��������
public:
	Cat(std::string _owner, std::string _name, int _age, bool _gender, double _tail_length) // �����������
		: owner(_owner), name(_name), age(_age), gender(_gender), tail_length(_tail_length) {}

	std::string get_owner() const { // ��������� ����� ���������
		return owner;
	}
	std::string get_name() const{ // ��������� ������ ����
		return name;
	}
	int get_age() const{ // ��������� �������� ����
		return age;
	}
	bool get_gender() const {
		return gender;
	}
	double get_tail_length() const{ // ��������� ����� ������ ����
		return tail_length;
	}

	bool get_deleted() const { // ��������� ����� ��������
		return deleted;
	}

	bool operator==(const Cat& other) const { // �������� ��������� ���� �������
		return name == other.get_name() && age == other.get_age() && gender == other.get_gender() && 
			tail_length == other.get_tail_length();
	}

	void set_deleted() { // �������, ��� ������ �����
		deleted = true;
	}
};

class HashTable {
	std::vector <Cat*> table; // ���-������� �������, ���� - ��� ���������, �������� - ��������� �� ��������� "���"
	size_t capacity; // ������ �������
	size_t size; // ���-�� �������������� ���������

	size_t Hash(const std::string& key) const { // ���-�������, ���������� ������ murmur ��� �������� �����
		const int seed = 5381; // ������� �����
		const int m = 33;     // ����������������� �����������
		const int r = 16;     // ���������� ��� ��� ������

		int len = key.length();
		int hash = seed ^ len;
		const char* data = &key[0];

		// ��������� �������� ����� (�� 8 ��������)
		for (int i = 0; i < len; ++i) {
			hash = (hash * m) ^ data[i]; // ��������� � xor
		}

		// ��������� �������������
		hash ^= hash >> r;
		hash *= m;
		hash ^= hash >> r;

		return hash % capacity;
	}

	long long get_index(const std::string& key) const{ // ��������� ������� � ���-������� �� �����
		size_t index = Hash(key); // �������� ������, ����� �� ������ ���� ��� ����� ��������
		for (size_t i = 0; i < capacity; ++i) {
			size_t probe_index = (index + i) % capacity; // �������� ������������
			if (table[probe_index] == nullptr) // ���� ����� ������, �� ���� ��� ����� ������� - �������� ��� � �������
				return -1;
			if (!table[probe_index]->get_deleted() && table[probe_index]->get_owner() == key) // ���������, ��� ����� ��������� � ������ �� �����
				return probe_index;
		}
		return -1;
	}

	void rehash() { // ����������, ����� ������� ����������� �� 75%
		size_t new_capacity = capacity * 2;
		std::vector <Cat*> new_table (new_capacity, nullptr);
		for (size_t i = 0; i < capacity; i++) {
			if (table[i] != nullptr && !table[i]->get_deleted()) { // ��������� �� �������� ��������
				size_t new_index = Hash(table[i]->get_owner());
				while (new_table[new_index] != nullptr) 
					new_index = (new_index + 1) % new_capacity;
				new_table[new_index] = table[i];
			}
		}
		table = std::move(new_table); // ��������� ����� �������
		capacity = new_capacity;
	}

public:

	HashTable(size_t initial_size = 11) { // �����������, �� ��������� ������ 11 (�.�. ��������� ������� �����)
		capacity = initial_size;
		size = 0;
		for (size_t i = 0; i < capacity; ++i)
			table.push_back(nullptr);
	}

	~HashTable() { // ����������
		clear();
	}

	void insert(const std::string& key, Cat& value) { // ������� ��������
		size_t index = Hash(key);
		for (size_t i = 0; i < capacity; ++i) {
			size_t probe_index = (index + i) % capacity; // �������� ������������
			if (table[probe_index] == nullptr || table[probe_index]->get_deleted()) {
				table[probe_index] = &value; 
				size++; 
				if ((double)size / capacity >= 0.75) // �������� �������������
					rehash();
				return;
			}
			if (table[probe_index]->get_owner() == key) { // ���� ���� ��� ���� � ������� - ��������������
				table[probe_index] = &value;
				return;
			}
		}
	}

	bool remove(const std::string key) { // true - ���� ������� ������� �������, false - ���� ������� �� ������
		long long index = get_index(key);
		if (index != -1) {
			table[index]->set_deleted();
			size--;
			return true;
		}
		if (index == -1) {
			return false;
		}
	}

	bool find(const std::string& key, const Cat& value) const{ // ����� �������� �� �����
		long long index = get_index(key);
		if (index == -1 || table[index]->get_deleted() || table[index] != &value)
			return false;
		return true;
	}

	bool find(const std::string& key) const { // ����� �������� �� �����
		long long index = get_index(key);
		if (index == -1 || table[index]->get_deleted())
			return false;
		return true;
	}

	Cat& get(const std::string key) { // ��������� ������ �� ������ �� �������
		long long index = get_index(key);
		if (!find(key))
			throw std::runtime_error("No such element");
		return *(table[index]); // �������������� ���������, � ���������� ��������������� ��� � ������
	}

	void clear() { // ������� �������
		for (size_t i = 0; i < capacity; ++i) {
			if (table[i]) {
				table[i] = nullptr;
			}
		}
		size = 0;
	}

	size_t get_size () const{
		return size;
	}

	size_t get_capacity() const{
		return capacity;
	}

	bool empty() const{ // ��������, ����� �� �������
		return size == 0;
	}

	void print() const{ // ����� ������� �� �����
		for (int i = 0; i < capacity; i++)
		{
			std::cout << "table[" << i << "]" << " --> ";
			if (table[i] && !table[i]->get_deleted()) {
				std::cout << "Owner: " << table[i]->get_owner() << "; " << "Name: " << table[i]->get_name() << "; ";
				std::cout << "Age: " << table[i]->get_age() << " years; " << "Gender: ";
				if (table[i]->get_gender())
					std::cout << "female; ";
				else
					std::cout << "male; ";
				std::cout << "Tail lenght: " << table[i]->get_tail_length() << std::endl;
			}
			else {
				std::cout << "-" << std::endl;
			}
		}
	}

	//Cat* get_with_index(size_t index) {
	//	return (table[index]);
	//}

	//class Iterator {
	//	HashTable* table;
	//	size_t index;

	//	/*void skip_deleted() {
	//		while (index < table->get_capacity() && ) {
	//			if (table->get_with_index(index) != nullptr)
	//				++index;
	//		}
	//	}*/
	//public:

	//	Iterator(HashTable* table, size_t index)
	//		: table(table), index(index) {
	//		//skip_deleted();
	//	}

	//	Iterator& operator++() {
	//		++index;
	//		//skip_deleted();
	//		return *this;
	//	}

	//	Cat& operator*() {
	//		return *(table->get_with_index(index));
	//	}

	//	Cat* operator->() {
	//		return table->get_with_index(index);
	//	}

	//	bool operator!=(const Iterator& other) const {
	//		return index != other.index;
	//	}
	//};

	//Iterator begin() {
	//	return Iterator(this, 0);
	//}

	//Iterator end() {
	//	return Iterator(this, capacity);
	//}

};

void test() {
	Cat mur_1("John", "Murka", 2, 1, 2.5);
	Cat  mur_2("Paul", "Michelle", 1, 1, 3);
	Cat  mur_3("Ringo", "Star", 7, 0, 4.43);
	Cat  mur_4("George", "Bass", 5, 0, 2.2);
	Cat  mur_5("Martin", "Shadow", 2, 0, 5.0);
	HashTable h1;
	h1.insert(mur_1.get_owner(), mur_1);
	h1.insert(mur_2.get_owner(), mur_2);
	h1.insert(mur_3.get_owner(), mur_3);
	h1.insert(mur_4.get_owner(), mur_4);
	h1.insert(mur_5.get_owner(), mur_5);
	h1.print();
	std::cout << h1.find("John", mur_1) << std::endl;
	h1.remove("John");
	std::cout << h1.find("John", mur_1) << std::endl;
	h1.print();
	std::cout << h1.empty() << std::endl;
	try {
		auto kotik = h1.get("Ringo");
		std::cout << kotik.get_name() << std::endl;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Error: " << error.what() << std::endl;
	}
	std::cout << h1.get_size() << std::endl;
	h1.clear();
	std::cout << h1.empty() << std::endl;
	h1.print();
	std::cout << h1.find("Martin", mur_5) << std::endl;
	std::cout << h1.find("George", mur_5) << std::endl;
	try {
		auto kotik = h1.get("Ringo");
		std::cout << kotik.get_name() << std::endl;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Error: " << error.what() << std::endl;
	}
}

//void test_iterator() {
//	HashTable table;
//	Cat cat1("Alice", "Whiskers", 3, 15.5);
//	Cat cat2("Bob", "Shadow", 5, 13.2);
//	Cat cat3("Alice", "Fluffy", 2, 14.0);
//
//	table.insert(cat1.get_owner(), cat1);
//	table.insert(cat2.get_owner(), cat2);
//	table.insert(cat3.get_owner(), cat3);
//
//	for (auto it = table.begin(); it != table.end(); ++it) {
//		std::cout << it->get_owner() << " owns " << it->get_name() << std::endl;
//	}
//}

int main() {
	test();
	//test_iterator();
}