#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int NUM_SIZE = 9;
const long RADIX = 1e9;
int max(int a, int b){
	if(a > b)
		return a;
	return b;
}

struct num{
	int size;
	long* digits;
	int sign;
	int floatIndex;
};

int indexOf(char* str, char ch){
	int found = 0;
	int idx = -1;
	for(int i = 0; i < strlen(str) && !found; i++){
		if(str[i] == ch){
			found = 1;
			idx = i;
		}
	}
	return idx;
}

void reverse(char* s){
	int len = strlen(s);
	for(int i = 0; i < len / 2; i++){
		char t = s[i];
		s[i] = s[len - i -1];
		s[len - i - 1] = t;
	}
}

void printNum(struct num* n){
	printf("SIZE: %d, SIGN: %d, FLOAT_INDEX:%d\n", n->size, n->sign, n->floatIndex);
	for(int i = 0; i < n->size; i++)
		printf("%ld\t", n->digits[i]);
	printf("\n");
}

char* trim(char* str){
	char* s = (char*)malloc(strlen(str) * sizeof(char));
	int i = 1;
	for(i = 1; str[i] != '\0'; i++)
		s[i - 1] = str[i];
	s[i - 1] = '\0';
	return s;
}

char* concat(char* tgt, char* app){
	char* s = (char*) malloc((strlen(tgt) + strlen(app) + 1) * sizeof(char));
	memcpy(s, tgt, strlen(tgt));
	memcpy(s + strlen(tgt), app, strlen(app));
	s[strlen(tgt) + strlen(app)] = '\0';
	return s;
}

struct num makeNum(char* str){
	int start = 0, i, j, k;
	struct num n;
	if(str[0] == '-'){
		str = trim(str);
		n.sign = -1;
	}
	else if(str[0] == '+'){
		str = trim(str);
		n.sign = +1;
	}
	else{
		n.sign = +1;
	}

	int floatIndex = indexOf(str, '.');
	if(floatIndex == -1)
		str = concat(str, ".0");

	floatIndex = indexOf(str, '.');

	int len = strlen(str);

	int left = floatIndex, right = len - floatIndex - 1;
	n.floatIndex = (right - 1)/ 9 + 1;
	int size = ((left - 1) / 9 + (right - 1) / 9 + 2) * sizeof(long);
	n.size = size / sizeof(long);
	n.digits = (long*)malloc(size); 

	char* before = calloc(floatIndex + 1, sizeof(char));
	char* after = calloc(len - floatIndex , sizeof(char));
	before = memcpy(before, str, floatIndex);
	before[floatIndex] = '\0';
	after = memcpy(after, str + floatIndex + 1, len - floatIndex - 1);
	after[len - floatIndex -1] = '\0';
	reverse(before);


	char* temp = (char*)calloc(10, sizeof(char));

	for(i = 0, j = 0, k = n.floatIndex ; i < floatIndex; i++){
		if(j == NUM_SIZE){
			temp[9] = '\0';
			int l = strlen(temp);
			n.digits[k] = atoll(temp);
			k++;
			j = 0;
			i--;
		}
		else{
			temp[8 - j] = before[i];
			j++;
		}
	}
	if(j != 0){
		char* remaining = calloc(NUM_SIZE + 1, sizeof(char));
		memcpy(remaining + 9 - j, temp + 9 - j, j);
		for(int c = 0; c < 9 - j; c++)
			remaining[c] = '0';
		remaining[NUM_SIZE] = '\0';
		n.digits[k] = atoll(remaining);
	}

	free(temp);
	temp = (char*)calloc(10, sizeof(char));
	for(i = 0, j = 0, k = n.floatIndex - 1; i < len - floatIndex - 1; i++){
		if(j == NUM_SIZE ){;
			temp[9] = '\0';
			n.digits[k] = atoll(temp);
			k--;
			j = 0;
			i--;
			free(temp);
			temp = calloc(NUM_SIZE + 1, sizeof(char));
		}
		else{
			temp[j] = after[i];
			j++;
		}
	}
	if(j != 0){
		char* remaining = calloc(NUM_SIZE + 1, sizeof(char));
		remaining = memcpy(remaining, temp, NUM_SIZE + 1);
		for(int c = j; c < NUM_SIZE; c++)
			remaining[c] = '0';
		remaining[NUM_SIZE] = '\0';
		n.digits[k] = atoll(remaining);
	}
	return n;
}

void shiftLeft(struct num* n, int offSet){
	long* digits = (long*)calloc((n->size + offSet), sizeof(long));
	memcpy(digits + offSet, n->digits, n->size * sizeof(long));
	n->digits = digits;
	n->size += offSet;
	n->floatIndex += offSet;
}

void shiftRight(struct num* n, int offSet){
	long* digits = (long*)calloc((n->size + offSet), sizeof(long));
	memcpy(digits, n->digits, n->size * sizeof(long));
	n->digits = digits;
	n->size += offSet;
}

struct num* addNum(struct num* n1, struct num* n2){
	struct num* add = (struct num*)(malloc(sizeof(struct num)));
	if(n1->floatIndex < n2->floatIndex)
		shiftLeft(n1, n2->floatIndex - n1->floatIndex);
	else if(n1->floatIndex > n2->floatIndex)
		shiftLeft(n2, n1->floatIndex - n2->floatIndex);
	add->sign = n1->sign;
	add->floatIndex = n1->floatIndex;
	add->size = max(n1->size, n2->size) + 1;
	add->digits = (long*)calloc(add->size, sizeof(long));
	long a, b, carry = 0;
	for(int i = 0; i < add->size - 1; i++){
		if(i >= n1->size)
			a = 0;
		else
			a = n1->digits[i];
		if(i >= n2->size)
			b = 0;
		else
			b = n2->digits[i];

		long s = (a + b + carry);
		add->digits[i] = s % RADIX;
		carry = s / RADIX;
	}
	if(carry != 0)
		add->digits[add->size - 1] = carry;
	else
		add->size -= 1;
	return add;
}


void multRadix(struct num* n1, int n){
	if(n > 0){
		shiftLeft(n1, n);
		n1->floatIndex -= n;
	}
	else{
		shiftRight(n1, -n);
		n1->floatIndex -= n;
	}
}

int greater(struct num* n1, struct num* n2){ // true if n1 >= n2 else 0
	if(n1->size - n1->floatIndex > n2->size - n2->floatIndex)
		return 1;
	else if(n2->size - n2->floatIndex > n1->size - n1->floatIndex)
		return 0;
	else
		return (n1->digits[n1->size - 1] >= n2->digits[n2->size - 1]);
}

struct num* subNum(struct num* n1, struct num* n2){
	struct num* sub = (struct num*)(malloc(sizeof(struct num)));
	if(n1->floatIndex < n2->floatIndex)
		shiftLeft(n1, n2->floatIndex - n1->floatIndex);
	else if(n1->floatIndex > n2->floatIndex)
		shiftLeft(n2, n1->floatIndex - n2->floatIndex);
	if(greater(n1, n2)){
		sub->floatIndex = n1->floatIndex;
		sub->sign = 1;
		sub->size = max(n1->size, n2->size);
		sub->digits = (long*)calloc(sub->size, sizeof(long));
		long a, b, borrow = 0;
		for(int i = 0; i < sub->size; i++){
			if(i >= n1->size)
				a = 0;
			else
				a = n1->digits[i];
			if(i >= n2->size)
				b = 0;
			else
				b = n2->digits[i];

			int toBorrow;
			if(a - borrow < b){
				a = a + RADIX;
				toBorrow = 1;
			}
			else
				toBorrow = 0;
			long s = (a - b - borrow);
			sub->digits[i] = s % RADIX;
			borrow = toBorrow;
		}
	}
	else{
		sub = subNum(n2, n1);
		sub->sign = -1;
	}
	return sub;
}

char* toString(struct num* n){
	int size = n->size * NUM_SIZE + 1;
	int end = n->size * NUM_SIZE + 1;
	char* s = (char*)calloc(end + 1, sizeof(char));
	s[end] = '\0';
	end--;
	for(int i = 0; i < n->size; i++){
		if(i == n->floatIndex){
			s[end] = '.';
			end--;
		}
		char buffer[10];
		char* str = (char*)calloc(10, sizeof(char));
		snprintf(buffer, 10, "%ld", n->digits[i]);
		int len = strlen(buffer);
		//printf("CHECK%s\n", buffer);
		if(len != 9){
			memcpy(str + 9 - len, buffer, len + 1);
			for(int i = 0; i < 9 - len; i++)
				str[i] = '0';
			//printf("BUFF%s", str);
		}
		else
			str = buffer;
		for(int j = 8; j >= 0; j--){
			s[end] = str[j];
			end--;
		}
	}
	if(n->sign == -1) {
		char* trimmed = calloc(size + 2, sizeof(char));
		memcpy(trimmed + 1, s, size + 1);
		trimmed[0] = '-';
		return trimmed;
	}
	char* trimmed = calloc(size + 1, sizeof(char));
	memcpy(trimmed, s, size + 1);
	return trimmed;
}

void multLong(struct num* n1, long l, struct num* mult){
	mult->sign = n1->sign;
	mult->size = n1->size + 1;
	mult->floatIndex = n1->floatIndex;
	mult->digits = (long*)calloc(n1->size + 1, sizeof(long));
	long carry = 0;
	for(int i = 0; i < n1->size + 1; i++){
		long prod;
		if(i == n1->size){
			if(carry == 0){
				mult->size--;
			}
			else{
				prod = carry;
				mult->digits[i] = prod % RADIX;
			}
		}
		else{
			prod = l * n1->digits[i] + carry;
			mult->digits[i] = prod % RADIX;
			carry = prod / RADIX;
		}
	}
}

struct num* multNum(struct num* n1, struct num* n2){
	struct num* mult = (struct num*)malloc(sizeof(struct num));
	mult->size = n1->size + n2->size;
	mult->digits = (long*)calloc(mult->size, sizeof(long));
	if(n1->sign == n2->sign)
		mult->sign = 1;
	else
		mult->sign = -1;
	mult->floatIndex = n1->floatIndex + n2->floatIndex;
	
	for(int i = 0; i < n2->size; i++){
		struct num* temp = (struct num*)malloc(sizeof(struct num));
		struct num* acc = (struct num*)malloc(sizeof(struct num));
		multLong(n1, n2->digits[i], temp);
		multRadix(temp, i - n2->floatIndex);
		mult = addNum(mult, temp);
	}
	return mult;
}

struct num * abs(struct num* n){
	struct num* ab = (struct num*)malloc(sizeof(struct num));
	ab->size = n->size;
	ab->sign = 1;
	ab->digits = (long*)malloc(sizeof(long) * n->size);
	memcpy(ab->digits, n->digits, n->size);
	ab->floatIndex = n->floatIndex;
	return ab;
}

int main(int argc, char* argv){
	char* s1 = "-95098341.00000000000000000000";
	char* s2 = "0.00000000000000001";
	struct num n1 = makeNum(s1);
	struct num n2 = makeNum(s2);
	printNum(&n1);
	printNum(&n2);
	struct num* sub = subNum(&n1, &n2);

	printf("%s\n", toString(sub));
	struct num* mult = multNum(&n1, &n2);
	printf("%s\n", toString(mult));
}