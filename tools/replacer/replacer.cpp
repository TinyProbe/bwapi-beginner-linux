#include <iostream>
#include <map>

static std::pair<std::string, std::string> disassemble(std::string s);

int main(int ac, char **av) {
    if (ac < 2) {
        std::cerr << "usage:                                                    " << '\n';
        std::cerr << "    ./replacer < <filepath> 'variable = value' [ ... ]    " << '\n';
        std::cerr << "                                                          " << std::endl;
        return -1;
    }

    std::map<std::string, std::string> repl;
    for (int i = 1; i < ac; ++i) {
        auto kv = disassemble(av[i]);
        repl[kv.first] = kv.second;
    }

    std::string s;
    while (std::cin) {
        std::getline(std::cin, s);

        if (!std::isalpha(s.front())) {
            std::cout << s << '\n';
            continue;
        }

        auto kv = disassemble(s);
        std::string const &val = repl[kv.first];
        if (val == "") {
            std::cout << s << '\n';
        } else {
            std::cout << kv.first + " = " + val << '\n';
        }
    }
    std::cout << std::endl;
}

static std::pair<std::string, std::string> disassemble(std::string s) {
    std::size_t l = 0, r = 0;
    while (r < s.size()) {
        if (!iswspace(s[r])) s[l++] = s[r];
        ++r;
    }
    s.resize(l);

    size_t cutpos = s.find("=");
    if (cutpos == s.npos) {
        throw std::invalid_argument("invalid argument");
    }

    std::string k = s.substr(0, cutpos);
    std::string v = s.substr(cutpos + 1, s.size() - (cutpos + 1));
    return std::make_pair(k, v);
}
