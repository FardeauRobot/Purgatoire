#ifndef ITER_HPP
# define ITER_HPP

template<typename A, typename L, typename F>
void iter (A address, const L length, F function) {
    L i = 0;
    while (i < length) {
        function(address[i]);
        i++;
    }
}

// class iter {
//     public:
//         iter();
//         iter(const iter &src);
//         iter& operator= (const iter &other);
//         ~iter();
// };

#endif
