// File: minimangol/parsing/tokenize/test_lexer.c
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../minishell.h"
#include <cmocka.h> // Ensure the CMocka library is included

// Mock functions that might be needed
void ft_putstr_fd(char *s, int fd) {
	(void)s;
	(void)fd;
}

char *ft_strdup(const char *s) {
	char *dup = strdup(s);
	return dup;
}

char *ft_substr(char const *s, unsigned int start, size_t len) {
	if (!s)
		return NULL;
	size_t s_len = strlen(s);
	if (start >= s_len)
		return strdup("");
	if (len > s_len - start)
		len = s_len - start;
	char *result = malloc(len + 1);
	if (!result)
		return NULL;
	memcpy(result, s + start, len);
	result[len] = '\0';
	return result;
}

int ft_strcmp(const char *s1, const char *s2) {
	return strcmp(s1, s2);
}

// Setup and teardown functions
static int setup(void **state) {
	(void)state;
	return 0;
}

static int teardown(void **state) {
	(void)state;
	return 0;
}

// Helper function to compare tokens
static void assert_token(t_token *token, int expected_type, const char *expected_value) {
	assert_non_null(token);
	assert_int_equal(token->type, expected_type);
	assert_string_equal(token->value, expected_value);
}

// Test empty input
static void test_tokenize_empty_input(void **state) {
	(void)state;
	t_token *tokens = tokenize("");
	assert_null(tokens);
	// free_tokens should not be called on NULL
}

// Test simple word tokenization
static void test_tokenize_simple_word(void **state) {
	(void)state;
	t_token *tokens = tokenize("hello");
	assert_token(tokens, TOKEN_WORD, "hello");
	assert_null(tokens->next);
	free_tokens(tokens);
}

// Test multiple words
static void test_tokenize_multiple_words(void **state) {
	(void)state;
	t_token *tokens = tokenize("hello world");
	assert_token(tokens, TOKEN_WORD, "hello");
	assert_token(tokens->next, TOKEN_WORD, "world");
	assert_null(tokens->next->next);
	free_tokens(tokens);
}

// Test operators
static void test_tokenize_operators(void **state) {
	(void)state;
	t_token *tokens = tokenize("|&<>()");
	assert_token(tokens, TOKEN_PIPE, "|");
	assert_token(tokens->next, TOKEN_AND, "&");
	assert_token(tokens->next->next, TOKEN_REDIR_IN, "<");
	assert_token(tokens->next->next->next, TOKEN_REDIR_OUT, ">");
	assert_token(tokens->next->next->next->next, TOKEN_LPAREN, "(");
	assert_token(tokens->next->next->next->next->next, TOKEN_RPAREN, ")");
	assert_null(tokens->next->next->next->next->next->next);
	free_tokens(tokens);
}

// Test complex operators
static void test_tokenize_complex_operators(void **state) {
	(void)state;
	t_token *tokens = tokenize("|| && >> <<");
	assert_token(tokens, TOKEN_OR, "||");
	assert_token(tokens->next, TOKEN_AND, "&&");
	assert_token(tokens->next->next, TOKEN_APPEND, ">>");
	assert_token(tokens->next->next->next, TOKEN_HEREDOC, "<<");
	free_tokens(tokens);
}

// Test quoted strings
static void test_tokenize_quoted_strings(void **state) {
	(void)state;
	t_token *tokens = tokenize("\"hello world\" 'single quotes'");
	assert_token(tokens, TOKEN_WORD, "hello world");
	assert_token(tokens->next, TOKEN_WORD, "single quotes");
	free_tokens(tokens);
}

// Test escaped characters in quotes
static void test_tokenize_escaped_quotes(void **state) {
	(void)state;
	t_token *tokens = tokenize("\"hello \\\"world\\\"\"");
	assert_token(tokens, TOKEN_WORD, "hello \"world\"");
	free_tokens(tokens);
}

// Test mixed input
static void test_tokenize_mixed_input(void **state) {
	(void)state;
	t_token *tokens = tokenize("echo \"hello world\" | grep 'hello'");
	assert_token(tokens, TOKEN_WORD, "echo");
	assert_token(tokens->next, TOKEN_WORD, "hello world");
	assert_token(tokens->next->next, TOKEN_PIPE, "|");
	assert_token(tokens->next->next->next, TOKEN_WORD, "grep");
	assert_token(tokens->next->next->next->next, TOKEN_WORD, "hello");
	free_tokens(tokens);
}

// Test syntax error checking
static void test_check_syntax_errors(void **state) {
	(void)state;
	// Test balanced parentheses
	t_token *tokens1 = tokenize("(echo hello)");
	assert_int_equal(check_syntax_errors(tokens1), 0);
	free_tokens(tokens1);
	
	// Test unbalanced parentheses (too many opening)
	t_token *tokens2 = tokenize("((echo hello)");
	assert_int_equal(check_syntax_errors(tokens2), 1);
	free_tokens(tokens2);
	
	// Test unbalanced parentheses (too many closing)
	t_token *tokens3 = tokenize("(echo hello))");
	assert_int_equal(check_syntax_errors(tokens3), 1);
	free_tokens(tokens3);
	
	// Test operator at end
	t_token *tokens4 = tokenize("echo |");
	assert_int_equal(check_syntax_errors(tokens4), 1);
	free_tokens(tokens4);
}

// Test handle_operator function
static void test_handle_operator(void **state) {
	(void)state;
	t_token *head = NULL;
	t_token *curr = NULL;
	char input[] = "||";
	int i = 0;

	handle_operator(&head, &curr, input, &i);
	assert_token(head, TOKEN_OR, "||");
	assert_int_equal(i, 2);
	assert_null(head->next);
	free_tokens(head);
}

// Test handle_quotes function
static void test_handle_quotes(void **state) {
	(void)state;
	t_token *head = NULL;
	t_token *curr = NULL;
	char input[] = "\"quoted text\"";
	int i = 0;

	handle_quotes(&head, &curr, input, &i);
	assert_token(head, TOKEN_WORD, "quoted text");
	assert_null(head->next);
	free_tokens(head);
}

// Test handle_word function
static void test_handle_word(void **state) {
	(void)state;
	t_token *head = NULL;
	t_token *curr = NULL;
	char input[] = "word|";
	int i = 0;

	handle_word(&head, &curr, input, &i);
	assert_token(head, TOKEN_WORD, "word");
	assert_null(head->next);
	assert_int_equal(i, 4);
	free_tokens(head);
}

// Main function to run all tests
int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_tokenize_empty_input),
		cmocka_unit_test(test_tokenize_simple_word),
		cmocka_unit_test(test_tokenize_multiple_words),
		cmocka_unit_test(test_tokenize_operators),
		cmocka_unit_test(test_tokenize_complex_operators),
		cmocka_unit_test(test_tokenize_quoted_strings),
		cmocka_unit_test(test_tokenize_escaped_quotes),
		cmocka_unit_test(test_tokenize_mixed_input),
		cmocka_unit_test(test_check_syntax_errors),
		cmocka_unit_test(test_handle_operator),
		cmocka_unit_test(test_handle_quotes),
		cmocka_unit_test(test_handle_word),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}