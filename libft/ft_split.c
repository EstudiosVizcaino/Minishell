#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static char	*get_word(char const *s, char c, int *pos)
{
	int		start;
	char	*word;

	while (s[*pos] == c)
		(*pos)++;
	start = *pos;
	while (s[*pos] && s[*pos] != c)
		(*pos)++;
	word = ft_substr(s, start, *pos - start);
	return (word);
}

char	**ft_split(char const *s, char c)
{
	char	**result;
	int		words;
	int		i;
	int		pos;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	result = malloc((words + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	i = 0;
	pos = 0;
	while (i < words)
	{
		result[i] = get_word(s, c, &pos);
		if (!result[i])
		{
			while (i-- > 0)
				free(result[i]);
			free(result);
			return (NULL);
		}
		i++;
	}
	result[i] = NULL;
	return (result);
}
