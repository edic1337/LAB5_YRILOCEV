#pragma once
#include "index.h"

// чтобы красиво показать результат пользователю
typedef struct {
    int  doc_id;  /* id документа, где нашлось слово */ 
    char title[MAX_TITLE_LEN];  /* заголовок вопроса */
    int  score;  /* score показывает, насколько хорошо документ подходит под запрос */
} SearchResult;

// ответ целиком 
typedef struct {
    Vector* results;   /* elements: SearchResult, топ-10 */
    int     total;     /* всего найдено документов */
    double  time_ms;  /* время поиска */
} SearchResults;

// функция найдет совпадение в двух викторах и вернет виктор общих элементов 
Vector* intersectPostings(Vector** lists, int n);

/* получает готовый индекс (загруженное дерево) и сырую строку запроса, внутри себя она делает:
Разбивает запрос на отдельные токены
Ищет каждый токен в дереве
Вызывает intersectPostings для объединения списков документов
Вычисляет score и сортирует документы от лучших к худшим
Упаковывает топ-10 результатов в структуру SearchResults и возвращает её
*/
SearchResults* search(Index* idx, const char* query);

// вывод
void printResultsText(const SearchResults* sr);
// вывод
void printResultsJSON(const SearchResults* sr);

// очистка
void freeSearchResults(SearchResults* sr);
