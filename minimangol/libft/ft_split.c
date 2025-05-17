/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 09:44:03 by achat             #+#    #+#             */
/*   Updated: 2025/04/12 22:27:11 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_tokens(const char *s, char c);
static size_t	fller(char **tokens_v, const char *s, char c);
static size_t	ft_free(char **tokens_v, int position, size_t buffer);

char	**ft_split(const char *s, char c)
{
	size_t	tokens;
	char	**tokens_v;

	if (NULL == s)
		return (NULL);
	tokens = 0;
	tokens = count_tokens(s, c);
	tokens_v = ft_calloc(tokens + 1, sizeof(char *));
	if (!tokens_v)
		return (NULL);
	tokens_v[tokens] = NULL;
	if (fller(tokens_v, s, c))
		return (NULL);
	return (tokens_v);
}

static size_t	ft_free(char **tokens_v, int position, size_t buffer)
{
	int	i;

	i = 0;
	tokens_v[position] = malloc(buffer);
	if (NULL == tokens_v[position])
	{
		while (i < position)
		{
			free(tokens_v[i++]);
		}
		free(tokens_v);
		return (1);
	}
	return (0);
}

static size_t	count_tokens(const char *s, char delimiter) {
    size_t tokens = 0;
    int inside_token = 0;

    while (*s) {
        // Skip delimiters
        while (*s == delimiter && *s) s++;

        // Process non-delimiter characters
        inside_token = 0;
        while (*s != delimiter && *s) {
            if (*s == '(' || *s == ')') {
                if (inside_token) tokens++; // End previous token
                tokens++; // Count parenthesis as a token
                inside_token = 0;
                s++;
            } else {
                if (!inside_token) {
                    tokens++;
                    inside_token = 1;
                }
                s++;
            }
        }
    }
    return tokens;
}

// Modified fller to handle parentheses splitting
static size_t	fller(char **tokens_v, const char *s, char c) {
    size_t len;
    int i = 0;
    const char *start;

    while (*s) {
        // Skip delimiters
        while (*s == c && *s) s++;
        start = s;
        len = 0;

        while (*s != c && *s) {
            if (*s == '(' || *s == ')') {
                // Save current token if any
                if (len > 0) {
                    if (ft_free(tokens_v, i, len + 1)) return 1;
                    ft_strlcpy(tokens_v[i++], start, len + 1);
                }
                // Save parenthesis
                if (ft_free(tokens_v, i, 2)) return 1;
                tokens_v[i][0] = *s;
                tokens_v[i++][1] = '\0';
                s++;
                start = s;
                len = 0;
            } else {
                len++;
                s++;
            }
        }

        // Save remaining part
        if (len > 0) {
            if (ft_free(tokens_v, i, len + 1)) return 1;
            ft_strlcpy(tokens_v[i++], start, len + 1);
        }
    }
    return 0;
}
