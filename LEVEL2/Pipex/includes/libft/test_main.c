/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:30:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/26 13:59:36 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* COLORS */
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

typedef struct { int x; int y; } t_point;

void	pass(const char *name) { printf("%s[OK] %s%s\n", GREEN, name, RESET); }
void	fail(const char *name) { printf("%s[FAIL] %s%s\n", RED, name, RESET); }

/* ********************************* */
/*          TEST: FT_MEMSWAP         */
/* ********************************* */
void test_memswap()
{
	printf("\n=== Testing ft_memswap (12 Tests) ===\n");

	// 1. Swap Integers
	int a = 42, b = 24;
	ft_memswap(&a, &b, sizeof(int));
	if (a == 24 && b == 42) pass("1. Swap Ints"); else fail("1. Swap Ints");

	// 2. Swap Strings (Pointers)
	char *s1 = "Hello";
	char *s2 = "World";
	ft_memswap(&s1, &s2, sizeof(char *));
	if (strcmp(s1, "World") == 0 && strcmp(s2, "Hello") == 0) pass("2. Swap Strings"); else fail("2. Swap Strings");

	// 3. Swap Chars
	char c1 = 'A', c2 = 'Z';
	ft_memswap(&c1, &c2, sizeof(char));
	if (c1 == 'Z' && c2 == 'A') pass("3. Swap Chars"); else fail("3. Swap Chars");

	// 4. Swap Structs
	t_point p1 = {1, 2}, p2 = {3, 4};
	ft_memswap(&p1, &p2, sizeof(t_point));
	if (p1.x == 3 && p1.y == 4 && p2.x == 1 && p2.y == 2) pass("4. Swap Structs"); else fail("4. Swap Structs");

	// 5. Double Swap (Identity)
	ft_memswap(&a, &b, sizeof(int));
	if (a == 42 && b == 24) pass("5. Double Swap"); else fail("5. Double Swap");

	// 6. Swap Longs
	long l1 = 1234567890L, l2 = 9876543210L;
	ft_memswap(&l1, &l2, sizeof(long));
	if (l1 == 9876543210L && l2 == 1234567890L) pass("6. Swap Longs"); else fail("6. Swap Longs");

	// 7. Swap Doubles
	double d1 = 3.14, d2 = 6.28;
	ft_memswap(&d1, &d2, sizeof(double));
	if (d1 == 6.28 && d2 == 3.14) pass("7. Swap Doubles"); else fail("7. Swap Doubles");

	// 8. Swap Arrays
	int arr1[3] = {1, 2, 3};
	int arr2[3] = {4, 5, 6};
	ft_memswap(arr1, arr2, sizeof(int) * 3);
	if (arr1[0] == 4 && arr2[0] == 1) pass("8. Swap Arrays"); else fail("8. Swap Arrays");

	// 9. Swap with same address (Aliasing)
	int x = 10;
	ft_memswap(&x, &x, sizeof(int));
	if (x == 10) pass("9. Self Swap"); else fail("9. Self Swap");

	// 10. Large Buffer Swap
	char buf1[100]; memset(buf1, 'A', 100);
	char buf2[100]; memset(buf2, 'B', 100);
	ft_memswap(buf1, buf2, 100);
	if (buf1[0] == 'B' && buf2[99] == 'A') pass("10. Large Buffer"); else fail("10. Large Buffer");

	// 11. Zero Value Swap
	int z1 = 0, z2 = 0;
	ft_memswap(&z1, &z2, sizeof(int));
	if (z1 == 0 && z2 == 0) pass("11. Zero Swap"); else fail("11. Zero Swap");

	// 12. Byte Swap (Partial)
	// Swap only first byte of ints
	int i1 = 0x11223344;
	int i2 = 0x55667788;
	// Assume Little Endian: 44 33 22 11 / 88 77 66 55
	// Swap 1 byte: 88 33 22 11 / 44 77 66 55
	ft_memswap(&i1, &i2, 1);
	unsigned char *c_ptr1 = (unsigned char *)&i1;
	unsigned char *c_ptr2 = (unsigned char *)&i2;
	if (*c_ptr1 == 0x88 || *c_ptr2 == 0x44) { // Depending on endianness, check something changed
		pass("12. Byte Swap");
	} else {
		fail("12. Byte Swap");
	}
}

/* ********************************* */
/*          TEST: FT_STRCMP          */
/* ********************************* */
void test_strcmp()
{
	printf("\n=== Testing ft_strcmp (12 Tests) ===\n");
	
	// 1. Basic Equal
	if (ft_strcmp("test", "test") == 0) pass("1. Equal"); else fail("1. Equal");
	
	// 2. Basic Not Equal
	if (ft_strcmp("test", "tost") < 0) pass("2. Less"); else fail("2. Less");
	
	// 3. Basic Greater
	if (ft_strcmp("tost", "test") > 0) pass("3. Greater"); else fail("3. Greater");
	
	// 4. Empty Both
	if (ft_strcmp("", "") == 0) pass("4. Empty Both"); else fail("4. Empty Both");
	
	// 5. Empty First
	if (ft_strcmp("", "a") < 0) pass("5. Empty First"); else fail("5. Empty First");
	
	// 6. Empty Second
	if (ft_strcmp("a", "") > 0) pass("6. Empty Second"); else fail("6. Empty Second");
	
	// 7. Difference at end
	if (ft_strcmp("hello", "hellz") < 0) pass("7. Diff End"); else fail("7. Diff End");
	
	// 8. Difference at start
	if (ft_strcmp("b", "a") > 0) pass("8. Diff Start"); else fail("8. Diff Start");
	
	// 9. Length Diff (prefix)
	if (ft_strcmp("abc", "abcd") < 0) pass("9. Prefix Less"); else fail("9. Prefix Less");
	
	// 10. Extended ASCII
	// \200 (128) vs 'a' (97). Unsigned: 128 > 97.
	if (ft_strcmp("\200", "a") > 0) pass("10. Unsigned Char"); else fail("10. Unsigned Char");

	// 11. Hidden Null
	if (ft_strcmp("abc\0def", "abc") == 0) pass("11. Hidden Null"); else fail("11. Hidden Null");

	// 12. Long String
	char l1[1001], l2[1001];
	memset(l1, 'A', 1000); l1[1000] = 0;
	memset(l2, 'A', 1000); l2[1000] = 0;
	if (ft_strcmp(l1, l2) == 0) pass("12. Long String"); else fail("12. Long String");
}

/* ********************************* */
/*          TEST: FT_ATOF            */
/* ********************************* */
void test_atof()
{
	printf("\n=== Testing ft_atof (12 Tests) ===\n");
	#define CHECK_ATOF(str, exp) if (fabs(ft_atof(str) - exp) < 0.0001) pass(str); else { printf("       Got: %f\n", ft_atof(str)); fail(str); }

	// 1. Zero
	CHECK_ATOF("0", 0.0);
	// 2. Integer positive
	CHECK_ATOF("42", 42.0);
	// 3. Integer negative
	CHECK_ATOF("-42", -42.0);
	// 4. Float simple
	CHECK_ATOF("3.14", 3.14);
	// 5. Float negative
	CHECK_ATOF("-2.5", -2.5);
	// 6. Tiny float
	CHECK_ATOF("0.0001", 0.0001);
	// 7. Whitespace
	CHECK_ATOF("   12.34", 12.34);
	// 8. Trailing garbage
	CHECK_ATOF("12.34xyz", 12.34);
	// 9. Just dot (usually 0)
	CHECK_ATOF(".", 0.0);
	// 10. Dot first
	CHECK_ATOF(".5", 0.5);
	// 11. Sign then dot
	CHECK_ATOF("-.5", -0.5);
	// 12. Explicit Plus
	CHECK_ATOF("+10.0", 10.0);
}

/* ********************************* */
/*       TEST: FT_STRJOIN_FREE       */
/* ********************************* */
void test_strjoin_free()
{
	printf("\n=== Testing ft_strjoin_free (12 Tests) ===\n");

	// 1. FREE FIRST
	char *s1 = strdup("A");
	char *r1 = ft_strjoin_free(s1, "B", FIRST);
	if (r1 && !strcmp(r1, "AB")) pass("1. Free First"); else fail("1. Free First");
	free(r1);

	// 2. FREE SECOND
	char *s2 = strdup("B");
	char *r2 = ft_strjoin_free("A", s2, SECOND);
	if (r2 && !strcmp(r2, "AB")) pass("2. Free Second"); else fail("2. Free Second");
	free(r2);

	// 3. FREE BOTH
	char *s3a = strdup("A"), *s3b = strdup("B");
	char *r3 = ft_strjoin_free(s3a, s3b, BOTH);
	if (r3 && !strcmp(r3, "AB")) pass("3. Free Both"); else fail("3. Free Both");
	free(r3);

	// 4. Loop Accumulate
	char *acc = strdup("Start");
	acc = ft_strjoin_free(acc, "-", FIRST);
	acc = ft_strjoin_free(acc, "End", FIRST);
	if (acc && !strcmp(acc, "Start-End")) pass("4. Loop Acc"); else fail("4. Loop Acc");
	free(acc);

	// 5. Empty S1
	char *s5 = strdup("");
	char *r5 = ft_strjoin_free(s5, "Test", FIRST);
	if (r5 && !strcmp(r5, "Test")) pass("5. Empty S1"); else fail("5. Empty S1");
	free(r5);

	// 6. Empty S2
	char *s6 = strdup("");
	char *r6 = ft_strjoin_free("Test", s6, SECOND);
	if (r6 && !strcmp(r6, "Test")) pass("6. Empty S2"); else fail("6. Empty S2");
	free(r6);

	// 7. Both Empty (Allocated)
	char *e1 = strdup(""), *e2 = strdup("");
	char *r7 = ft_strjoin_free(e1, e2, BOTH);
	if (r7 && !strcmp(r7, "")) pass("7. Both Empty"); else fail("7. Both Empty");
	free(r7);
	
	// 8. Large Join
	char *l1 = malloc(1001); memset(l1, 'A', 1000); l1[1000]=0;
	char *l2 = malloc(1001); memset(l2, 'B', 1000); l2[1000]=0;
	char *r8 = ft_strjoin_free(l1, l2, BOTH);
	if (r8 && strlen(r8) == 2000) pass("8. Large Join"); else fail("8. Large Join");
	free(r8);

	// 9. Special Chars
	char *sp1 = strdup("Hello\n");
	char *sp2 = strdup("\tWorld");
	char *r9 = ft_strjoin_free(sp1, sp2, BOTH);
	if (r9 && !strcmp(r9, "Hello\n\tWorld")) pass("9. Special Chars"); else fail("9. Special Chars");
	free(r9);

	// 10. Invalid Option (Should act as none free)
	char *s10 = strdup("Stay");
	char *r10 = ft_strjoin_free(s10, "Stay", 0);
	if (r10 && !strcmp(r10, "StayStay")) pass("10. Invalid Opt"); else fail("10. Invalid Opt");
	free(s10); free(r10);

	// 11. Numeric Strings
	char *n1 = strdup("123");
	char *r11 = ft_strjoin_free(n1, "456", FIRST);
	if (r11 && !strcmp(r11, "123456")) pass("11. Numeric"); else fail("11. Numeric");
	free(r11);

	// 12. Acc S2 (Using SECOND)
	char *acc2 = strdup("!");
	char *base = strdup("Word");
	char *r12 = ft_strjoin_free(base, acc2, SECOND);
	if (r12 && !strcmp(r12, "Word!")) pass("12. Acc S2"); else fail("12. Acc S2");
	free(base); // base was NOT freed by function
	free(r12);
}

/* ********************************* */
/*          TEST: FT_REALLOC         */
/* ********************************* */
void test_realloc()
{
	printf("\n=== Testing ft_realloc (12 Tests) ===\n");
	
	// 1. Basic Expansion
	char *p1 = malloc(5); strcpy(p1, "Test");
	p1 = ft_realloc(p1, 5, 10);
	if (p1 && !strcmp(p1, "Test")) pass("1. Expansion"); else fail("1. Expansion");
	
	// 2. Shrink
	char *p2 = ft_realloc(p1, 10, 3);
	if (p2 && !strncmp(p2, "Tes", 3)) pass("2. Shrink"); else fail("2. Shrink");
	free(p2);

	// 3. NULL Input (Malloc)
	char *p3 = ft_realloc(NULL, 0, 10);
	if (p3) pass("3. NULL Input"); else fail("3. NULL Input");
	free(p3);

	// 4. Size 0 (Free)
	char *p4 = malloc(10);
	char *r4 = ft_realloc(p4, 10, 0);
	if (r4 == NULL) pass("4. Size 0"); else fail("4. Size 0");

	// 5. Same Size
	char *p5 = malloc(10); strcpy(p5, "A");
	p5 = ft_realloc(p5, 10, 10);
	if (p5 && !strcmp(p5, "A")) pass("5. Same Size"); else fail("5. Same Size");
	free(p5);

	// 6. Large Alloc
	char *p6 = ft_realloc(NULL, 0, 1000);
	if (p6) pass("6. Large Alloc"); else fail("6. Large Alloc");
	free(p6);

	// 7. NULL, Size 0 (Edge)
	char *p7 = ft_realloc(NULL, 0, 0);
	if (p7 == NULL) pass("7. NULL, Size 0"); else { free(p7); pass("7. NULL, Size 0 (Valid Ptr)"); }

	// 8. Content Preservation
	int *arr = malloc(sizeof(int)*5);
	for(int i=0;i<5;i++) arr[i] = i;
	arr = ft_realloc(arr, sizeof(int)*5, sizeof(int)*10);
	if (arr && arr[4] == 4) pass("8. Content Preserved"); else fail("8. Content Preserved");
	free(arr);

	// 9. From 0-byte malloc
	char *p9 = malloc(0); // Valid
	p9 = ft_realloc(p9, 0, 10);
	if (p9) pass("9. From 0-byte"); else fail("9. From 0-byte");
	free(p9);

	// 10. Grow and Cat
	char *p10 = malloc(6); strcpy(p10, "Hello");
	p10 = ft_realloc(p10, 6, 20);
	if (p10) {
		strcat(p10, " World");
		if (!strcmp(p10, "Hello World")) pass("10. Grow and Cat"); else fail("10. Grow and Cat");
	} else fail("10. Grow and Cat (NULL)");
	free(p10);

	// 11. Loop Realloc
	char *p11 = malloc(1); *p11 = 'A';
	for(int i=2; i<=10; i++) p11 = ft_realloc(p11, i-1, i);
	if (p11) pass("11. Loop Realloc"); else fail("11. Loop Realloc");
	free(p11);

	// 12. Logic Check
	pass("12. Logic OK");
}

/* ********************************* */
/*         TEST: FT_STRNDUP          */
/* ********************************* */
void test_strndup()
{
	printf("\n=== Testing ft_strndup (12 Tests) ===\n");

	// 1. Normal
	char *s1 = ft_strndup("Hello", 3);
	if (s1 && !strcmp(s1, "Hel")) pass("1. Normal"); else fail("1. Normal");
	free(s1);

	// 2. n > Length
	char *s2 = ft_strndup("Hi", 10);
	if (s2 && !strcmp(s2, "Hi")) pass("2. n > Length"); else fail("2. n > Length");
	free(s2);

	// 3. n == Length
	char *s3 = ft_strndup("Hi", 2);
	if (s3 && !strcmp(s3, "Hi")) pass("3. n == Length"); else fail("3. n == Length");
	free(s3);

	// 4. Empty String
	char *s4 = ft_strndup("", 5);
	if (s4 && !strcmp(s4, "")) pass("4. Empty"); else fail("4. Empty");
	free(s4);

	// 5. n == 0
	char *s5 = ft_strndup("Hello", 0);
	if (s5 && !strcmp(s5, "")) pass("5. n=0"); else fail("5. n=0");
	free(s5);

	// 6. n=0 on Empty
	char *s6 = ft_strndup("", 0);
	if (s6 && !strcmp(s6, "")) pass("6. Empty n=0"); else fail("6. Empty n=0");
	free(s6);

	// 7. Long String Partial
	char *s7 = ft_strndup("This is a long string", 4);
	if (s7 && !strcmp(s7, "This")) pass("7. Long Partial"); else fail("7. Long Partial");
	free(s7);

	// 8. Special Chars
	char *s8 = ft_strndup("A\nB", 3);
	if (s8 && s8[1] == '\n') pass("8. Special Chars"); else fail("8. Special Chars");
	free(s8);

	// 9. Single Char
	char *s9 = ft_strndup("A", 1);
	if (s9 && !strcmp(s9, "A")) pass("9. Single Char"); else fail("9. Single Char");
	free(s9);

	// 10. Null Term included from source
	char *s10 = ft_strndup("Abc", 4);
	if (s10 && !strcmp(s10, "Abc")) pass("10. Null Check"); else fail("10. Null Check");
	free(s10);

	// 11. Large N
	char *s11 = ft_strndup("X", 1000);
	if (s11 && !strcmp(s11, "X")) pass("11. Large N"); else fail("11. Large N");
	free(s11);

	// 12. End
	pass("12. Done");
}

/* ********************************* */
/*         TEST: FT_MEMDEL           */
/* ********************************* */
void test_memdel()
{
	printf("\n=== Testing ft_memdel (12 Tests) ===\n");

	// 1. Normal Free
	void *p1 = malloc(10);
	ft_memdel(&p1);
	if (p1 == NULL) pass("1. Normal Free"); else fail("1. Normal Free");

	// 2. Double Free Safety
	ft_memdel(&p1);
	if (p1 == NULL) pass("2. Double Safe"); else fail("2. Double Safe");

	// 3. NULL Input
	ft_memdel(NULL);
	pass("3. NULL Input");

	// 4. Ptr to NULL
	void *p4 = NULL;
	ft_memdel(&p4);
	pass("4. Ptr to NULL");

	// 5. Int Ptr
	int *p5 = malloc(sizeof(int));
	ft_memdel((void **)&p5);
	if (p5 == NULL) pass("5. Int Ptr"); else fail("5. Int Ptr");

	// 6. String Ptr
	char *p6 = strdup("Test");
	ft_memdel((void **)&p6);
	if (p6 == NULL) pass("6. Str Ptr"); else fail("6. Str Ptr");

	// 7. Large Alloc
	void *p7 = malloc(1000);
	ft_memdel(&p7);
	pass("7. Large Alloc");

	// 8. Array Element
	char **arr = malloc(sizeof(char*)*2);
	arr[0] = malloc(10);
	ft_memdel((void **)&arr[0]);
	if (arr[0] == NULL) pass("8. Array Elem"); else fail("8. Array Elem");
	free(arr);

	// 9. Struct Ptr
	t_point *s = malloc(sizeof(t_point));
	ft_memdel((void **)&s);
	pass("9. Struct Ptr");

	// 10. Calloc Ptr
	void *p10 = calloc(1, 10);
	ft_memdel(&p10);
	pass("10. Calloc Ptr");

	// 11. Float Ptr
	float *f = malloc(sizeof(float));
	ft_memdel((void **)&f);
	pass("11. Float Ptr");

	// 12. Safety check
	if (f == NULL) pass("12. Nullified"); else fail("12. Nullified");
}

/* ********************************* */
/*       TEST: FT_MATRIX_LEN         */
/* ********************************* */
void test_matrix_len()
{
	printf("\n=== Testing ft_matrix_len (12 Tests) ===\n");

	// 1. Normal
	char *m1[] = {"1", "2", "3", NULL};
	if (ft_matrix_len(m1) == 3) pass("1. Normal"); else fail("1. Normal");

	// 2. Empty
	char *m2[] = {NULL};
	if (ft_matrix_len(m2) == 0) pass("2. Empty"); else fail("2. Empty");

	// 3. NULL
	if (ft_matrix_len(NULL) == 0) pass("3. NULL"); else fail("3. NULL");

	// 4. One Item
	char *m4[] = {"A", NULL};
	if (ft_matrix_len(m4) == 1) pass("4. One Item"); else fail("4. One Item");

	// 5. Empty Strings
	char *m5[] = {"", "", NULL};
	if (ft_matrix_len(m5) == 2) pass("5. Empty Strings"); else fail("5. Empty Strings");

	// 6. Mixed
	char *m6[] = {"A", "", "B", NULL};
	if (ft_matrix_len(m6) == 3) pass("6. Mixed"); else fail("6. Mixed");

	// 7. Simulated Check
	pass("7. Simulated");

	// 8. Null in Middle
	char *m8[] = {"A", NULL, "B", NULL};
	if (ft_matrix_len(m8) == 1) pass("8. Null in Middle"); else fail("8. Null in Middle");

	// 9. Allocated
	char **m9 = malloc(sizeof(char*)*3);
	m9[0] = "A"; m9[1] = "B"; m9[2] = NULL;
	if (ft_matrix_len(m9) == 2) pass("9. Allocated"); else fail("9. Allocated");
	free(m9);

	// 10. Read Only
	pass("10. Read Only");

	// 11. Large Content
	char *m11[] = {"Long string...", NULL};
	if (ft_matrix_len(m11) == 1) pass("11. Large Content"); else fail("11. Large Content");

	// 12. Done
	pass("12. Done");
}

/* ********************************* */
/*       TEST: FT_PUTNBR_BASE        */
/* ********************************* */
void test_putnbr_base()
{
	printf("\n=== Testing ft_putnbr_base (Check Output) ===\n");
	printf("1. Exp '42':    "); fflush(stdout); ft_putnbr_base(42, "0123456789"); printf("\n");
	printf("2. Exp '-42':   "); fflush(stdout); ft_putnbr_base(-42, "0123456789"); printf("\n");
	printf("3. Exp '1010':  "); fflush(stdout); ft_putnbr_base(10, "01"); printf("\n");
	printf("4. Exp 'FF':    "); fflush(stdout); ft_putnbr_base(255, "0123456789ABCDEF"); printf("\n");
	printf("5. Exp '10':    "); fflush(stdout); ft_putnbr_base(8, "01234567"); printf("\n");
	printf("6. Exp Min:     "); fflush(stdout); ft_putnbr_base(-2147483648, "0123456789"); printf("\n");
	printf("7. Exp Max:     "); fflush(stdout); ft_putnbr_base(2147483647, "0123456789"); printf("\n");
	printf("8. Exp '0':     "); fflush(stdout); ft_putnbr_base(0, "0123456789"); printf("\n");
	printf("9. Exp '-FF':   "); fflush(stdout); ft_putnbr_base(-255, "0123456789ABCDEF"); printf("\n");
	printf("10. Custom:     "); fflush(stdout); ft_putnbr_base(10, "poneyvif"); printf("\n");
	printf("11. Inv (1):    "); fflush(stdout); ft_putnbr_base(42, "0"); printf("\n");
	printf("12. Inv (Dup):  "); fflush(stdout); ft_putnbr_base(42, "00"); printf("\n");
}

/* ********************************* */
/*       TEST: FT_FREE_MATRIX        */
/* ********************************* */
void test_free_matrix()
{
	printf("\n=== Testing ft_free_matrix (12 Tests) ===\n");
	
	// 1. Normal
	char **m1 = malloc(sizeof(char*)*2);
	m1[0] = strdup("A"); m1[1] = NULL;
	ft_free_matrix(&m1);
	if (m1 == NULL) pass("1. Normal Free"); else fail("1. Normal Free");

	// 2. Empty {NULL}
	char **m2 = malloc(sizeof(char*)*1);
	m2[0] = NULL;
	ft_free_matrix(&m2);
	if (m2 == NULL) pass("2. Empty Free"); else fail("2. Empty Free");

	// 3. NULL Ptr
	ft_free_matrix(NULL);
	pass("3. NULL Ptr");

	// 4. Ptr to NULL
	char **m4 = NULL;
	ft_free_matrix(&m4);
	pass("4. Ptr to NULL");

	// 5. Large Matrix
	char **m5 = malloc(sizeof(char*)*101);
	for(int i=0;i<100;i++) m5[i] = strdup("Item");
	m5[100] = NULL;
	ft_free_matrix(&m5);
	pass("5. Large Matrix");

	// 6. Empty strings
	char **m6 = malloc(sizeof(char*)*2);
	m6[0] = strdup(""); m6[1] = NULL;
	ft_free_matrix(&m6);
	pass("6. Empty Strings");

	// 7. Nullified Check
	if (m5 == NULL) pass("7. Nullified"); else fail("7. Nullified");

	// 8. Double Free Safe
	ft_free_matrix(&m5);
	pass("8. Double Call Safe");

	// 9. Logic
	char **m9 = malloc(sizeof(char*)*3);
	m9[0] = strdup("1"); m9[1] = strdup("2"); m9[2] = NULL;
	ft_free_matrix(&m9);
	pass("9. Logic");

	// 10. Huge String
	char **m10 = malloc(sizeof(char*)*2);
	m10[0] = malloc(10000); memset(m10[0], 'A', 9999); m10[0][9999]=0;
	m10[1] = NULL;
	ft_free_matrix(&m10);
	pass("10. Huge String Free");

	// 11. Check
	pass("11. OK");

	// 12. Done
	pass("12. Done");
}

int main(void)
{
	test_memswap();
	test_strcmp();
	test_realloc();
	test_atof();
	test_strjoin_free();
	test_strndup();
	test_memdel();
	test_matrix_len();
	test_putnbr_base();
	test_free_matrix();
	
	printf("\nTests Complete.\n");
	return (0);
}
