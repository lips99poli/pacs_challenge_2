# SparseMatrix Class

La classe `SparseMatrix` è una rappresentazione di una matrice sparsa. Questa classe fornisce diversi metodi per manipolare e interagire con la matrice.

## Traits

File header con le dichiarazioni dei tipi della matrice. In particolare sono da notare le costruzioni dei due container della matrice, per lo stato `UNCOMPRESSED` e `COMPRESSED`, rispettivamente una mappa con overload dell' `operator<` per la chiave e un custom container con 3 vettori al suo interno. 

## Metodi

### `SparseMatrix(size_type R = 0, size_type C = 0, const uncompressed_container_type& init = {})`

Questo è il costruttore di `SparseMatrix`. Crea una nuova matrice sparsa con le dimensioni specificate.

**Implementazione:**

Controlla le dimensioni e le ingrandisce in caso ci siano elementi con indici maggiori delle dimensioni.

### `size_type get_n_rows() const`

Questo metodo restituisce il numero di righe nella matrice.

**Implementazione:**

Trivial.

### `size_type get_n_cols() const`

Questo metodo restituisce il numero di colonne nella matrice.

**Implementazione:**

Trivial.

### `std::vector<T> get_row(size_type r) const`

Questo metodo restituisce una riga specifica dalla matrice come un `std::vector`.

**Implementazione:**

Chiama il metodo template privato extract_vector specificando il template parameter a Row.

### `std::vector<T> get_col(size_type c) const`

Questo metodo restituisce una colonna specifica dalla matrice come un `std::vector`.

**Implementazione:**

Chiama il metodo template privato extract_vector specificando il template parameter a Col

### `template<ExtractOptions RorC> std::vector<T> extract_vector(size_type k) const`

Questo metodo estrae un vettore specifico dalla matrice.

**Implementazione:**

Metodo template che può estrarre vettori dalla matrice di tipo T, sia righe, sia colonne in funzione di un template parameter `RorC`

### `inline bool is_compressed() const`

Questo metodo verifica se la matrice è compressa.

**Implementazione:**

Trivial

### `void resize(size_type R, size_type C)`

Questo metodo ridimensiona la matrice alle dimensioni specificate.

**Implementazione:**

Se le dimensioni in input sono minori e la matrice è nello stato `UNCOMPRESSED` allora elimina gli elementi che strabordano.

### `void compress()`

Questo metodo comprime la matrice.

**Implementazione:**

Cambia la modalità con cui la matrice è salvata in memoria. Interessante l'ultizzo del template parameter `StorageOptions` per evitare if statements nel codice. Si sfrutta la conversione in int e il suo utilizzo per estrarre il corrispondente valore all'interno della chiave della mappa. Per il corretto funzionamento ho dovuto salvare oltre allo StorageOptions corretto della matrice anche il suo opposto. (membro `otherSO` static constexpr)

### `void uncompress()`

Questo metodo decomprime la matrice.

**Implementazione:**

Esegue l'inverso del metodo precedente, e sfrutta lo stesso accorgimento.

### `const T& operator() (const size_type r, const size_type c) const`

Questo operatore restituisce un elemento specifico dalla matrice.

**Implementazione:**

Metodo const per leggere elementi dentro la matrice, non modifica in alcun modo lo stato interno della matrice

### `T& operator()(const size_type r, const size_type c)`

Questo operatore modifica un elemento specifico nella matrice.

**Implementazione:**

Se gli indici fìdati sono presenti restituisco una reference al valore e posso leggerlo e scriverlo in entrmabi gli stati.
Se gli indici sono compatibili con le dimensioni ma non presenti e lo stato è `UNCOMPRESSED` aggiungo l'elemento e restituisco una reference così posso scriverlo.
Se gli indici sono compatibili con le dimensioni ma non presenti e lo stato è `COMPRESSED` non posso aggiungere l'elemento. Per non bloccare il programma restituisco la reference di un buffer in cui l'utente può scrivere senza danneggiare la matrice.

### `bool delete_element(const size_type r, const size_type c)`

Questo metodo elimina un elemento specifico dalla matrice.

**Implementazione:**

Siccome l'utente può aggiungere elementi coin il call operator sbagliando gli indici fornisco un metodo per rimuovere elementi indesiderati nello stato `UNCOMPRESSED`.

### `std::vector<T> operator*(const std::vector<T>& v) const`

Questo operatore moltiplica la matrice per un vettore.

**Implementazione:**

Gestisce i vari casi in modo diverso:
- `UNCOMPRESSED`: scorre la mappa e somma il risultato locale al vettore result nella posizione corretta
- `COMPRESSED`: due implementazioni diverse in funzione della StorageOptions
    - `RowMajor`: estrae le righe della matrice e usa `std::inner_product`
    - `ColMajor`: sfrutta il fatto che il risultato è una combinazione lineare delle colonne, sfruttando `std::transorm`

### `std::vector<std::vector<T>> operator*(const SparseMatrix<SO,T>& v) const`

Questo operatore moltiplica la matrice per un'altra matrice.

**Implementazione:**

Simile al precedente:
- `UNCOMPRESSED`: scorre la mappa delle lhs e se nella rhs ci sono elementi corrispondenti opportuni somma il risultato locale al vettore result nella posizione corretta
- `COMPRESSED`: estrae righe della lhs e colonne della rhs e sfrutta `std::inner_product` per fare i calcoli

### `template<NormOptions N> norm_type norm() const`

Questo metodo calcola la norma della matrice. Quale norma? Dipende dal template parameter: One, Infinity, Froebenius.

**Implementazione:**

Usa algoritmi della standard library combinati con lambda functions, e meccanismi di lettura già visti nei metodi precedenti.

## Friend functions

### `template<StorageOptions SO_gen, typename T_gen> friend SparseMatrix<SO_gen, T_gen> generate_random_matrix(size_type rows, size_type cols, size_type n_elements, T_gen min, T_gen max)`

Per generare una matrice con dimensioni date in input e tipo come template parameter.

### `template<StorageOptions SO_file, typename T_file> friend SparseMatrix<SO_file, T_file> read_matrix_from_file(const std::string& filename)`

Questo metodo legge una matrice da un file.

