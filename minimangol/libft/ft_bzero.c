/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:24:42 by achat             #+#    #+#             */
/*   Updated: 2024/11/01 19:14:27 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	char	*tmp_ptr;

	tmp_ptr = (char *)s;
	while (n > 0)
	{
		*(tmp_ptr)++ = 0;
		n--;
	}
}
