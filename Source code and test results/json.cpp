#include "json.hpp"

//Classe che implementa un lista templatizzata per memorizzare il tipo lista e il tipo dizionario di json
template<typename T>
class List
{
    friend class json;

    public:
        List();
        List(const List& copy);
        ~List();

        void reset();

        void append(const T& el);
        void prepend(const T& el);

        const List& operator=(const List& l);

    private:
        struct Cell
        {
            T info;
            Cell* next;
        };

        typedef Cell* Pcell;
        Pcell head;
        Pcell tail;
};

template<typename T>
List<T>::List()
{
    head = tail = nullptr;
}

template<typename T>
List<T>::List(const List& copy)
{
    head = tail = nullptr;

    Pcell pc = copy.head;
    while(pc != nullptr)
    {
        append(pc->info);
        pc = pc -> next;
    }
}

template<typename T>
List<T>::~List()
{
    reset();
}

template<typename T>
void List<T>::append(const T& el)
{
    Pcell new_cell = new Cell;
    new_cell -> info = el;
    new_cell -> next = nullptr;

    if(head == nullptr)
        head = new_cell;
    
    else
        tail -> next = new_cell;

    tail = new_cell;
}

template<typename T>
void List<T>::prepend(const T& el)
{
    Pcell new_cell = new Cell;
    new_cell -> info = el;
    new_cell -> next = head;

    if(tail == nullptr)
        tail = new_cell;

    head = new_cell;
}

template<typename T>
const List<T>& List<T>::operator=(const List& l)
{
    if(this != &l)
    {
        reset();  //Distrugge lo stato di this

        Pcell pc = l.head;
        while(pc != nullptr)
        {
            append(pc -> info);
            pc = pc -> next;
        }
    }

    return *this;
}

template<typename T>
void List<T>::reset()
{
    while(head != nullptr)
    {
        Pcell temp = head -> next;
        delete head;
        head = temp;
    }

    tail = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------

struct json::impl
{
    int type;  //1:null, 2:numero, 3:booleano, 4:stringa, 5:lista, 6:dizionario

    double n;
    bool b;
    std::string s;
    List<json> l;  
    List<std::pair<std::string, json>> d;  

    void make_empty();
};

json::json()
{
    pimpl = new impl;
    pimpl -> type = 1;
}

json::json(json const& copy)
{
    pimpl = new impl;

    pimpl -> type = copy.pimpl -> type;

    if(pimpl -> type == 2)
        pimpl -> n = copy.pimpl -> n;

    else if(pimpl -> type == 3)
        pimpl -> b = copy.pimpl -> b;

    else if(pimpl -> type == 4)
        pimpl -> s = copy.pimpl -> s;

    else if(pimpl -> type == 5)
        pimpl -> l = copy.pimpl -> l;

    else if(pimpl -> type == 6)
        pimpl -> d = copy.pimpl -> d;
}


json::json(json&& copy)
{
    pimpl = new impl;

    pimpl -> type = copy.pimpl -> type;

    if(pimpl -> type == 2)
        pimpl -> n = copy.pimpl -> n;

    else if(pimpl -> type == 3)
        pimpl -> b = copy.pimpl -> b;

    else if(pimpl -> type == 4)
        pimpl -> s = copy.pimpl -> s;

    else if(pimpl -> type == 5)
    {
        pimpl -> l.head = copy.pimpl -> l.head;
        pimpl -> l.tail = copy.pimpl -> l.tail;

        copy.pimpl -> l.head = nullptr;
        copy.pimpl -> l.tail = nullptr;
    }

    else if(pimpl -> type == 6)
    {
        pimpl -> d.head = copy.pimpl -> d.head;
        pimpl -> d.tail = copy.pimpl -> d.tail;

        copy.pimpl -> d.head = nullptr;
        copy.pimpl -> d.tail = nullptr;
    }
}

json::~json()
{
    delete pimpl;
}

json& json::operator=(json const& l)
{
    if(this != &l)
    { 
        pimpl -> make_empty();  //Distrugge lo stato di this

        pimpl -> type = l.pimpl -> type;

        if(pimpl -> type == 2)
            pimpl -> n = l.pimpl -> n;

        else if(pimpl -> type == 3)
            pimpl -> b = l.pimpl -> b;

        else if(pimpl -> type == 4)
            pimpl -> s = l.pimpl -> s;

        else if(pimpl -> type == 5)
            pimpl -> l = l.pimpl -> l;

        else if(pimpl -> type == 6)
            pimpl -> d = l.pimpl -> d;
    }

    return *this;
}

json& json::operator=(json&& l)
{
    if(this != &l)
    {
        pimpl -> make_empty();  //Distrugge lo stato di this

        pimpl -> type = l.pimpl -> type;

        if(pimpl -> type == 2)
            pimpl -> n = l.pimpl -> n;

        else if(pimpl -> type == 3)
            pimpl -> b = l.pimpl -> b;

        else if(pimpl -> type == 4)
            pimpl -> s = l.pimpl -> s;

        else if(pimpl -> type == 5)
        {
            pimpl -> l.head = l.pimpl -> l.head;
            pimpl -> l.tail = l.pimpl -> l.tail;

            l.pimpl -> l.head = nullptr;
            l.pimpl -> l.tail = nullptr;
        }

        else if(pimpl -> type == 6)
        {
            pimpl -> d.head = l.pimpl -> d.head;
            pimpl -> d.tail = l.pimpl -> d.tail;

            l.pimpl -> d.head = nullptr;
            l.pimpl -> d.tail = nullptr;
        }
    }

    return *this;
}

//***************************************************************************************************************************
//Funzioni booleane

bool json::is_list() const
{
    return (pimpl -> type == 5);
}

bool json::is_dictionary() const
{
    return (pimpl -> type == 6);
}
    
bool json::is_string() const
{
    return (pimpl -> type == 4);
}

bool json::is_number() const
{
    return (pimpl -> type == 2);
}

bool json::is_bool() const
{
    return (pimpl -> type == 3);
}
    
bool json::is_null() const
{
    return (pimpl -> type == 1);
}

//***************************************************************************************************************************
//Operatore []

json& json::operator[](std::string const& key)
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};
    
    List<std::pair<std::string, json>>::Pcell pc = (pimpl -> d).head;
    bool found = false;

    while(pc != nullptr && ! found)
    {
        if((pc -> info).first == key)
            found = true;

        else
            pc = pc -> next;
    }

    if(found == true)
        return (pc -> info).second;;


    std::pair<std::string, json> el(key, json());
    insert(el);

    return (((pimpl -> d).tail) -> info.second);  //Prende l'ultimo elemento (appena aggiunto), ne estrae il campo info, e del campo info prende solo l'oggetto json
}

json const& json::operator[](std::string const& key) const
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};
    
    List<std::pair<std::string, json>>::Pcell pc = (pimpl -> d).head;
    bool found = false;

    while(pc != nullptr && ! found)
    {
        if((pc -> info).first == key)
            found = true;

        else
            pc = pc -> next;
    }

    if(found == false)
        throw json_exception{"Error: key not found in dictionary of a constant json object."};

    return (pc -> info).second;
}

//***************************************************************************************************************************
//Funzioni get

double& json::get_number()
{
    if(!is_number())
        throw json_exception{"Error: json object is not of type number."};

    return pimpl -> n;
}

double const& json::get_number() const
{
    if(!is_number())
        throw json_exception{"Error: json object is not of type number."};

    return pimpl -> n;
}

bool& json::get_bool()
{
    if(!is_bool())
        throw json_exception{"Error: json object is not of type boolean."};

    return pimpl -> b;
}

bool const& json::get_bool() const
{
    if(!is_bool())
        throw json_exception{"Error: json object is not of type boolean."};

    return pimpl -> b;
}

std::string& json::get_string()
{
    if(!is_string())
        throw json_exception{"Error: json object is not of type string."};

    return pimpl -> s;
}

std::string const& json::get_string() const
{
    if(!is_string())
        throw json_exception{"Error: json object is not of type string."};

    return pimpl -> s;
}

//***************************************************************************************************************************
//Funzione ausiliaria per svuotare lista e dizionario

void json::impl::make_empty()
{
    if(type == 5)
        l.reset();
    
    else if(type == 6)
        d.reset();
}

//Funzioni set
void json::set_string(std::string const& x)
{
    pimpl -> make_empty();

    pimpl -> s = x;
    pimpl -> type = 4;
}

void json::set_bool(bool x)
{
    pimpl -> make_empty();

    pimpl -> b = x;
    pimpl -> type = 3;

}

void json::set_number(double x)
{
    pimpl -> make_empty();

    pimpl -> n = x;
    pimpl -> type = 2;
}

void json::set_null()
{
    pimpl -> make_empty();

    pimpl -> type = 1;
}

void json::set_list()
{
    pimpl -> make_empty();

    pimpl -> type = 5;  
}

void json::set_dictionary()
{
    pimpl -> make_empty();

    pimpl -> type = 6;  
}

//***************************************************************************************************************************
//Funzioni per inserimento in lista e dizionario

void json::push_front(json const& x)
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    (pimpl -> l).prepend(x);
}

void json::push_back(json const& x)
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    (pimpl -> l).append(x);
}

void json::insert(std::pair<std::string,json> const& x)
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};

    (pimpl -> d).append(x);
}

//***************************************************************************************************************************
//Iteratore per la lista

struct json::list_iterator
{
    public:
        list_iterator(const List<json>::Pcell& h);

        json::list_iterator& operator++();
        json::list_iterator operator++(int);
        json& operator*() const;
        json* operator->() const;
        bool operator==(const list_iterator& l) const;
        bool operator!=(const list_iterator& l) const;

    private:
        using iterator_category = std::forward_iterator_tag;
        using value_type = json;
        using pointer = json*;
        using reference = json&;

        List<json>::Pcell p;
};

json::list_iterator::list_iterator(const List<json>::Pcell& h)
{
    p = h;
}

json::list_iterator& json::list_iterator::operator++()
{
    p = p -> next;
    return *this;
}

json::list_iterator json::list_iterator::operator++(int)
{
    json::list_iterator temp = *this;
    p = p -> next;
    return temp;
}

json& json::list_iterator::operator*() const
{
    return p -> info;
}

json* json::list_iterator::operator->() const
{
    return &(p -> info);
}

bool json::list_iterator::operator==(const list_iterator& l) const
{
    return p == l.p;
}

bool json::list_iterator::operator!=(const list_iterator& l) const
{
    return p != l.p;
}

json::list_iterator json::begin_list()
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    list_iterator it((pimpl -> l).head);
    return it;
}

json::list_iterator json::end_list()
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    list_iterator it(nullptr);
    return it;
}

//***************************************************************************************************************************
//Iteratore costante per la lista

struct json::const_list_iterator
{
    public:
        const_list_iterator(const List<json>::Pcell& h);

        const const_list_iterator& operator++();
        const const_list_iterator operator++(int);
        const json& operator*() const;
        const json* operator->() const;
        bool operator==(const const_list_iterator& l) const;
        bool operator!=(const const_list_iterator& l) const;

    private:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const json;
        using pointer = const json*;
        using reference = const json&;

        const List<json>::Cell* p;
};


json::const_list_iterator::const_list_iterator(const List<json>::Pcell& h)
{
    p = h;
}

const json::const_list_iterator& json::const_list_iterator::operator++()
{
    p = p -> next;
    return *this;
}

const json::const_list_iterator json::const_list_iterator::operator++(int)
{
    const_list_iterator temp = *this;
    p = p -> next;
    return temp;
}

const json& json::const_list_iterator::operator*() const
{
    return p -> info;
}

const json* json::const_list_iterator::operator->() const
{
    return &(p -> info);
}

bool json::const_list_iterator::operator==(const const_list_iterator& l) const
{
    return p == l.p;
}

bool json::const_list_iterator::operator!=(const const_list_iterator& l) const
{
    return p != l.p;
}

json::const_list_iterator json::begin_list() const
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    const_list_iterator it((pimpl -> l).head);
    return it;
}

json::const_list_iterator json::end_list() const
{
    if(!is_list())
        throw json_exception{"Error: json object is not of type list."};

    const_list_iterator it(nullptr);
    return it;
}

//***************************************************************************************************************************
//Iteratore per il dizionario

struct json::dictionary_iterator
{
    public:
        dictionary_iterator(const List<std::pair<std::string,json>>::Pcell& h);

        dictionary_iterator& operator++();
        dictionary_iterator operator++(int);
        std::pair<std::string,json>& operator*() const;
        std::pair<std::string,json>* operator->() const;
        bool operator==(const dictionary_iterator& l) const;
        bool operator!=(const dictionary_iterator& l) const;

    private:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<std::string,json>;
        using pointer = std::pair<std::string,json>*;
        using reference = std::pair<std::string,json>&;

        List<std::pair<std::string,json>>::Pcell p;
};

json::dictionary_iterator::dictionary_iterator(const List<std::pair<std::string,json>>::Pcell& h)
{
    p = h;
}

json::dictionary_iterator& json::dictionary_iterator::operator++()
{
    p = p -> next;
    return *this;
}

json::dictionary_iterator json::dictionary_iterator::operator++(int)
{
    dictionary_iterator temp = *this;
    p = p -> next;
    return temp;
}

std::pair<std::string,json>& json::dictionary_iterator::operator*() const
{
    return p -> info;
}

std::pair<std::string,json>* json::dictionary_iterator::operator->() const
{
    return &(p -> info);
}

bool json::dictionary_iterator::operator==(const dictionary_iterator& l) const
{
    return p == l.p;
}

bool json::dictionary_iterator::operator!=(const dictionary_iterator& l) const
{
    return p != l.p;
}

json::dictionary_iterator json::begin_dictionary()
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};

    dictionary_iterator it((pimpl -> d).head);
    return it;
}

json::dictionary_iterator json::end_dictionary()
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};

    dictionary_iterator it(nullptr);
    return it;
}

//***************************************************************************************************************************
//Iteratore costante per il dizionario

struct json::const_dictionary_iterator
{
    public:
        const_dictionary_iterator(const List<std::pair<std::string,json>>::Pcell& h);

        const const_dictionary_iterator& operator++();
        const const_dictionary_iterator operator++(int);
        const std::pair<std::string,json>& operator*() const;
        const std::pair<std::string,json>* operator->() const;
        bool operator==(const const_dictionary_iterator& l) const;
        bool operator!=(const const_dictionary_iterator& l) const;

    private:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const std::pair<std::string,json>;
        using pointer = const std::pair<std::string,json>*;
        using reference = const std::pair<std::string,json>&;

        const List<std::pair<std::string,json>>::Cell* p;
};

json::const_dictionary_iterator::const_dictionary_iterator(const List<std::pair<std::string,json>>::Pcell& h)
{
    p = h;
}

const json::const_dictionary_iterator& json::const_dictionary_iterator::operator++()
{
    p = p -> next;
    return *this;
}

const json::const_dictionary_iterator json::const_dictionary_iterator::operator++(int)
{
    const_dictionary_iterator temp = *this;
    p = p -> next;
    return temp;
}

const std::pair<std::string,json>& json::const_dictionary_iterator::operator*() const
{
    return p -> info;
}

const std::pair<std::string,json>* json::const_dictionary_iterator::operator->() const
{
    return &(p -> info);
}

bool json::const_dictionary_iterator::operator==(const const_dictionary_iterator& l) const
{
    return p == l.p;
}

bool json::const_dictionary_iterator::operator!=(const const_dictionary_iterator& l) const
{
    return p != l.p;
}

json::const_dictionary_iterator json::begin_dictionary() const
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};

    const_dictionary_iterator it((pimpl -> d).head);
    return it;
}

json::const_dictionary_iterator json::end_dictionary() const
{
    if(!is_dictionary())
        throw json_exception{"Error: json object is not of type dictionary."};

    const_dictionary_iterator it(nullptr);
    return it;
}

//***************************************************************************************************************************
//Parser

void N(std::istream& lhs, json& rhs);  //Funzione per il parsing di null
void B(std::istream& lhs, json& rhs);  //Funzione per il parsing di un booleano
void Num(std::istream& lhs, json& rhs);  //Funzione per il parsing di un numero
void S(std::istream& lhs, json& rhs);  //Funzione per il parsing di una stringa
void D(std::istream& lhs, json& rhs, bool comma_before);  //Funzione per il parsing di un dizionario
void L(std::istream& lhs, json& rhs, bool comma_before);  //Funzione per il parsing di una lista. comma_before indica se prima dell'elemento della lista era presente una vigola
void J(std::istream& lhs, json& rhs, bool first);  //Funzione per la prima regola della grammatica. first indica se J viene chiamata per la prima volta

void consume_blanks(std::istream& lhs)  //Funzione ausiliaria per consumare i caratteri separatori
{
    while(lhs.peek() == ' ' || lhs.peek() == '\t' || lhs.peek() == '\n')
        lhs.ignore(1);
}


void N(std::istream& lhs, json& rhs)
{
    std::string a = "null";

    bool ans = true;
    int i = 0;

    while(i < 4 && ans)
    {
        if(lhs.get() != a.at(i))
            ans = false;

        else
            i++;
    }

    if(ans == false)
        throw json_exception{"Error while parsing json file\n-> Found a sequence of characters not allowed while parsing null"};

    rhs.set_null();

}

void B(std::istream& lhs, json& rhs)
{
    std::string a;

    if(lhs.peek() == 't')
        a = "true";

    else if(lhs.peek() == 'f')
        a = "false";

    bool ans = true;
    unsigned i = 0;

    while(i < a.size() && ans)
    {
        if(lhs.get() != a.at(i))
            ans = false;

        else
            i++;
    }

    if(!ans)
        throw json_exception{"Error while parsing json file\n-> Found a sequence of characters not allowed while parsing a boolean"};

    if(i == 4)
        rhs.set_bool(true);

    else if(i == 5)
        rhs.set_bool(false);
}

void Num(std::istream& lhs, json& rhs)
{
    if(lhs.peek() == '-')
    {
        lhs.ignore(1);
        char ch = lhs.peek();

        if(!isdigit(ch) && ch != '.')
            throw json_exception{"Error while parsing json file\n-> Found a sequence of characters not allowed while parsing a double"};

        lhs.putback('-');
    }

    double x;
    lhs >> x;

    rhs.set_number(x);
}

void S(std::istream& lhs, json& rhs)
{
    std::string a;

    while(!lhs.eof() && lhs.peek() != '"')
    {
        char ch = lhs.get();
        a.push_back(ch);

        if(ch == '\\' && lhs.peek() == '"')
            a.push_back(lhs.get());
    }

    if(lhs.eof())
        throw json_exception{"Error while parsing json file\n-> Reached end of file while parsing a string"};
    
    rhs.set_string(a);
}

void D(std::istream& lhs, json& rhs, bool comma_before)
{
    if(lhs.peek() != '}')
    {
        while(lhs.peek() == ',' || lhs.peek() == ' ' || lhs.peek() == '\t' || lhs.peek() == '\n')  //Ciclo per consumare tutti i caratteri separatori prima della prima coppia
        {
                if(lhs.peek() == ',')  //Se viene incontrata una virgola all'inizio, lancia eccezione
                    throw json_exception{"Error while parsing json file\n-> Found comma before the first pair of a dictionary"};

                lhs.ignore(1);
        }
    }

    if(comma_before && lhs.peek() == '}')  //Se ci troviamo in fondo al dizionario ma prima e' stata trovata una virgola, lancia eccezione
        throw json_exception{"Error while parsing json file\n-> Found comma followed by no pair in a dictionary"};

    if(lhs.peek() != '}')
    {   
        json key;
        J(lhs, key, false);

        if(!key.is_string()) 
             throw json_exception{"Error while parsing json file\n-> Found a key of a dictionary not of type string"};

        consume_blanks(lhs);
        
        if(lhs.peek() != ':')
            throw json_exception{"Error while parsing json file\n-> Colon not found after a key of a dictionary"};

        lhs.ignore(1);

        consume_blanks(lhs);

        json value;
        J(lhs, value, false);

        std::pair<std::string, json> p(key.get_string(), value); 
        rhs.insert(p);
    }

        int comma = 0;
        while(lhs.peek() == ',' || lhs.peek() == ' ' || lhs.peek() == '\t' || lhs.peek() == '\n')  //Consuma i caratteri successivi
        {
                if(lhs.peek() == ',') 
                    comma++;

            lhs.ignore(1);
        }   

        if(comma == 1 && lhs.eof())  //Se e' stata trovata una virgola ma si e' arrivati alla fine del file, lancia eccezione
            throw json_exception{"Error while parsing json file\n-> Found comma followed by no pair in a dictionary"};

        else if(comma == 1)
            D(lhs, rhs, true);

        else if(comma > 1)
            throw json_exception{"Error while parsing json file\n-> Found more than one comma after a pair of a dictionary"};

        else if(comma == 0 && !lhs.eof() && lhs.peek() != '}')  //Se non sono state trovate virgole e non siamo alla fine della lista, lancia eccezione. Il controllo su eof serve per evitare che venga lanciata questa eccezione se manca la parentesi quadra chiusa
            throw json_exception{"Error while parsing json file\n-> Comma not found after a pair of a dictionary"};
}

void L(std::istream& lhs, json& rhs, bool comma_before)
{   
    while(lhs.peek() == ',' || lhs.peek() == ' ' || lhs.peek() == '\t' || lhs.peek() == '\n')  //Ciclo per consumare tutti i caratteri separatori prima del primo valore
    {
        if(lhs.peek() == ',')  //Se viene incontrata una virgola all'inizio, lancia eccezione
            throw json_exception{"Error while parsing json file\n-> Found comma before the first value of a list"};

        lhs.ignore(1);
    }
    
    if(comma_before && lhs.peek() == ']')  //Se ci troviamo in fondo alla lista ma prima e' stata trovata una virgola, lancia eccezione
        throw json_exception{"Error while parsing json file\n-> Found comma followed by no value in a list"};


    if(lhs.peek() != ']')  //Se non siamo in fondo alla lista
    {    
        json x;
        J(lhs, x, false);
        rhs.push_back(x);

        int comma = 0;
        while(lhs.peek() == ',' || lhs.peek() == ' ' || lhs.peek() == '\t' || lhs.peek() == '\n')  //Consuma i caratteri successivi
        {
            if(lhs.peek() == ',') 
                comma++;

            lhs.ignore(1);
        }   

        if(comma == 1 && lhs.eof())  //Se e' stata trovata una virgola ma si e' arrivati alla fine del file, lancia eccezione
            throw json_exception{"Error while parsing json file\n-> Found comma followed by no value in a list"};

        else if(comma == 1)
            L(lhs, rhs, true);

        else if(comma > 1)
            throw json_exception{"Error while parsing json file\n-> Found more than one comma after an element of a list"};

        else if(comma == 0 && !lhs.eof() && lhs.peek() != ']')  //Se non sono state trovate virgole e non siamo alla fine della lista, lancia eccezione. Il controllo su eof serve per evitare che venga lanciata questa eccezione se manca la parentesi quadra chiusa
            throw json_exception{"Error while parsing json file\n-> Comma not found after an element of a list"};
    }
}

void J(std::istream& lhs, json& rhs, bool first)
{
    consume_blanks(lhs);  //Consuma i primi caratteri

    char ch = lhs.peek();

    if(!lhs.eof())
    {
        if(ch == '[')
        {
            lhs.ignore(1);  //Consuma la parentesi quadra aperta '['

            rhs.set_list();
            L(lhs, rhs, false);
    
            if(lhs.peek() != ']')
                throw json_exception{"Error while parsing json file\n-> Closing square bracket not found for list"};

            lhs.ignore(1);  //Consuma la parentesi quadra chiusa ']'
        }

        else if(ch == '{')
        {
            lhs.ignore(1);  //Consuma la parentesi graffa aperta '{'
            
            rhs.set_dictionary();
            D(lhs, rhs, false);

            if(lhs.peek() != '}')
                throw json_exception{"Error while parsing json file\n-> Closing brace not found for dictionary"};

            lhs.ignore(1);  //Consuma la parentesi graffa aperta '}'
        }

        else if(ch == '"')
        {
            lhs.ignore(1);  //Consuma i doppi apici '"'            
            S(lhs, rhs);
            lhs.ignore(1);  //Consuma i doppi apici '"'
        }

        else if(isdigit(ch) || ch == '-' || ch == '.')
        {
            Num(lhs, rhs);
        }

        else if(ch == 't' || ch == 'f')
        {
            B(lhs, rhs);
        }

        else if(ch == 'n')
        {
            N(lhs, rhs);
        }

        else
            throw json_exception{"Error while parsing json file\n-> Found a character not allowed"};
    }

    if(first)
    {
        consume_blanks(lhs);  //Consuma eventuali caratteri separatori dopo l'oggetto json piu' esterno

        if(!lhs.eof())
            throw json_exception{"Error while parsing json file\n-> Found a character not allowed"};
    }
}

std::istream& operator>>(std::istream& lhs, json& rhs)
{
    J(lhs, rhs, true);
    return lhs;
}

//***************************************************************************************************************************
//Stampa

void print_null(std::ostream& lhs);
void print_number(std::ostream& lhs, json const& rhs);
void print_bool(std::ostream& lhs, json const& rhs);
void print_string(std::ostream& lhs, json const& rhs);
void print_list(std::ostream& lhs, json const& rhs);
void print_dictionary(std::ostream& lhs, json const& rhs);

void print_null(std::ostream& lhs)
{
    lhs << "null";
}

void print_number(std::ostream& lhs, json const& rhs)
{
    lhs << rhs.get_number();
}

void print_bool(std::ostream& lhs, json const& rhs)
{
    if(rhs.get_bool() == true)
        lhs << "true";

    else
        lhs << "false";
}

void print_string(std::ostream& lhs, json const& rhs)
{
    lhs << "\"" << rhs.get_string() << "\"";
}

void print_list(std::ostream& lhs, json const& rhs)
{
        lhs << "[";

        bool end = false;

        json::const_list_iterator it = rhs.begin_list();

        while(!end)
    	{
            if(it != rhs.end_list())
            {
                lhs << *it;
                it++;
            }

            if(it == rhs.end_list())
            {
                lhs << "]";
                end = true;
            }

            else
                lhs << ", ";
        } 
}

void print_dictionary(std::ostream& lhs, json const& rhs)
{
    lhs << "{";

    bool end = false;

    json::const_dictionary_iterator it = rhs.begin_dictionary();

    while(!end)
    {
        if(it != rhs.end_dictionary())
        {
            lhs << "\"" << (*it).first << "\"" << " : " << (*it).second;
            it++;
        }

        if(it == rhs.end_dictionary())
        {
            lhs << "}";
            end = true;
        }

        else
            lhs << ", ";
    } 
}

std::ostream& operator<<(std::ostream& lhs, json const& rhs)
{
    if(rhs.is_null())
        print_null(lhs);

    else if(rhs.is_number())
        print_number(lhs, rhs);

    else if(rhs.is_bool())
        print_bool(lhs, rhs);

    else if(rhs.is_string())
        print_string(lhs, rhs);

    else if(rhs.is_list())
        print_list(lhs, rhs);

    else if(rhs.is_dictionary())
        print_dictionary(lhs, rhs);

    return lhs;
}
