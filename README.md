[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/BX65L5j-)
# Семинар 2

## Вариант 29 (Открытая адресация: линейное пробирование)


### Пользовательский класс данных
```c++
class Cat { // класс "Кот"
	int age; // возраст
	std::string name; // кличка кота
	std::string owner; // имя владельца
	bool gender; // пол кота
	double tail_length; // длина хвоста
	bool deleted = false; // флаг, является ли объект удалённым
```
* Так как поля класса по умолчанию private, то в методах класса реализованы геттеры (```get_owner()```, ```get_name()```, ```get_age()```, ```get_gender()```, ```get_tail_legth()```, ```get_deleted()```) для получения значения поля, без возможночсти его изменения;
* Реализован конструктор класса (использована краткая форма записи для улучшения читабельности кода)
  ```c++
  public:
	Cat(std::string _owner, std::string _name, int _age, bool _gender, double _tail_length) // конструктор
		: owner(_owner), name(_name), age(_age), gender(_gender), tail_length(_tail_length) {}
  ```
* Перегружен оператор ```==``` для корректного сравнения двух объектов данного класса;

  
  ```c++
  bool operator==(const Cat& other) const { // оператор сравнения двух котиков
	return name == other.get_name() && age == other.get_age() && gender == other.get_gender() && 
		tail_length == other.get_tail_length();
  }
  ```
* Добавлен сеттер ```set_deleted()```) для пометки удалённого элемента;
  ```c++
  void set_deleted() { // пометка, что объект удалён
	deleted = true;
  }
  ```
  ---
  

### Хэш-таблица
```c++
class HashTable {
	std::vector <Cat*> table; // хэш-таблица котиков, ключ - имя владельца, значение - указатель на структуру "Кот"
	size_t capacity; // размер таблицы
	size_t size; // кол-во действительных элементов
```
**Хэш-функция**
```c++
size_t Hash(const std::string& key) const { // хэш-функция, упрощённый аналог murmur для коротких строк
	const int seed = 5381; // простое число
	const int m = 33;     // мультипликативный коэффициент
	const int r = 16;     // количество бит для сдвига

	int len = key.length();
	int hash = seed ^ len;
	const char* data = &key[0];

	// обработка коротких строк (до 8 символов)
	for (int i = 0; i < len; ++i) {
		hash = (hash * m) ^ data[i]; // умножение и xor
	}

	// финальное перемешивание
	hash ^= hash >> r;
	hash *= m;
	hash ^= hash >> r;

	return hash % capacity;
}
```
Выбран способ хэширования на основе упрощённой функции Murmur для строк небольшой длины, т.к. ключ в данном случае - имя, скорее всего не очень длинное.  


*Преимущества*:
* Простая реализация;
* Высокая производительность за счёт оптимизации под короткие строки и использования только умножений, сдвигов и xor-ов, выполняемых за константное время;
* Устойчивость к коллизиям;
* Минимальные затраты памяти;
---


**Private-методы**
```c++
long long get_index(const std::string& key) const{ // получение индекса в хэш-таблице по ключу
	size_t index = Hash(key); // получаем индекс, каким он должен быть без учёта коллизий
	for (size_t i = 0; i < capacity; ++i) {
		size_t probe_index = (index + i) % capacity; // линейное пробирование
		if (table[probe_index] == nullptr) // если нашли пустую, до того как нашли элемент - элемента нет в таблице
			return -1;
		if (!table[probe_index]->get_deleted() && table[probe_index]->get_owner() == key) // проверяем, что ключи совпадают и объект не удалён
			return probe_index;
	}
	return -1;
}
```
Метод `get_index(key)` позволяет получить индекс элемента в хэш-таблице по ключу. Этот метод имеет модификатор доступа `prinate`, так как пользователь не должен знать, где в таблице лежит нужный ему объект, однако может для реализации некоторых других методов хэш-таблицы (например, `find(key)`).  
В этом методе также используется линейное пробирование, для поиска элемента в случе коллизии.  
Если элемент не найден функция вернёт искуственное значение -1, которое никак не может быть реальным индексом хэш-таблицы.


```c++
void rehash() { // рехэшируем, когда таблица заполняется на 75%
	size_t new_capacity = capacity * 2;
	std::vector <Cat*> new_table (new_capacity, nullptr);
	for (size_t i = 0; i < capacity; i++) {
		if (table[i] != nullptr && !table[i]->get_deleted()) { // переносим не удалённые элементы
			size_t new_index = Hash(table[i]->get_owner());
			while (new_table[new_index] != nullptr) 
				new_index = (new_index + 1) % new_capacity;
			new_table[new_index] = table[i];
		}
	}
	table = std::move(new_table); // переносим новую таблицу
	capacity = new_capacity;
}
```
Метод `rehash()` выполняет расширение размеров хэш-таблицы, когда она заполнена на 75% (стандартный коэффицент заполнения).  
Создаётся новая таблица, в которую под новым хэшем добавляются все не удалённые элементы из старой таблицы. Затем новая таблица перемещается на место старой.

---


**Конструктор и деструктор**

```c++
HashTable(size_t initial_size = 11) { // конструктор, по умолчанию размер 11 (т.к. небольшое простое число)
	capacity = initial_size;
	size = 0;
	for (size_t i = 0; i < capacity; ++i)
		table.push_back(nullptr);
}
```
Конструктор по умолчанию задаёт значение размера хэш-таблицы равным 11, так как это небольшое простое число, что должно минимизировать коллизии.



```c++
~HashTable() { // деструктор
	clear();
}
```
Деструктор вызывет функцию `clear()`, которая превращает каждую ячейку хэш-таблицы в `nullptr`.

---


**Функции insert и remove**  
```c++
void insert(const std::string& key, Cat& value) { // вставка элемента
	size_t index = Hash(key);
	for (size_t i = 0; i < capacity; ++i) {
		size_t probe_index = (index + i) % capacity; // линейное пробирование
		if (table[probe_index] == nullptr || table[probe_index]->get_deleted()) {
			table[probe_index] = &value; 
			size++; 
			if ((double)size / capacity >= 0.75) // проверка заполненности
				rehash();
			return;
		}
		if (table[probe_index]->get_owner() == key) { // если ключ уже есть в таблице - перезаписываем
			table[probe_index] = &value;
			return;
		}
	}
}
```
Функция ```insert(key, value)``` позволяет вставить элемент в хэш-таблицу.   
В соответствии с вариантом реализован метод линейного пробирования, т.е. при возникновении коллизии элемент будет вставлен в ближайшую от начального индекса свободную ячейку.  
Также осуществляется проверка заполнения таблицы, и если необходимо вызывется функция ```rehash()```.
Если такой ключ в таблице уже существет, то его значение будет изменено на новый value (т.е. указатель на объект типа "Cat")
```c++
bool remove(const std::string key) { // true - если удалось удалить элемент, false - если элемент не найден
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
```

