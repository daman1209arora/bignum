#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-----------------------forward declarations--------------------
struct num* subNum(struct num* n1, struct num* n2);
struct num* addNum(struct num* n1, struct num* n2);
int compareMgn(struct num* n1, struct num* n2);
//---------------------------------------------------------------


//-----------------------------constants-------------------------
const int NUM_SIZE = 9;
const long RADIX = 1e9;
//---------------------------------------------------------------


/*  num is the primary building block of this bignum library. 
    It contains the digits stored in the particular radix(in
    this case 1e9). Also, it separately stores the size of the
    number in terms of the number of digits. It stores the floating
    index location. 
    Encoding format:
        Let the number be (b0 b1 b2 ... bn) . (a0 a1 a2 ... am)
        In that case the digits points an array of this form:
            [am, am-1, am-2 ... a0 bn bn-1 .... b0]
            The floating index is set to (m + 1)
            The sign is +1 or -1
            The size is (n + 1) + (m + 1)
*/
struct num{
        int size;       
        long* digits;
        int sign;
        int floatIndex;
};








int max(int a, int b){ if(a > b) return a; else return b;}
int min(int a, int b){ if(a < b) return a; else return b;}


int indexOf(char* str, char ch){
        //returns the first index of character ch in string str
        int found = 0;
        int idx = -1;
	int i; 
        for(i = 0; i < strlen(str) && !found; i++){
                if(str[i] == ch){
                        found = 1;
                        idx = i;
                }
        }
        return idx;
}

void reverse(char* s){
        //returns reverse of string
        int len = strlen(s);
	int i = 0;
        for(i = 0; i < len / 2; i++){
                char t = s[i];
                s[i] = s[len - i -1];
                s[len - i - 1] = t;
        }
}

char* trim(char* str){
        //Removes the first character from the given string
        char* s = (char*)malloc(strlen(str) * sizeof(char));
        int i = 1;
        for(i = 1; str[i] != '\0'; i++)
                s[i - 1] = str[i];
        s[i - 1] = '\0';
        return s;
}

char* concat(char* tgt, char* app){
        //Appends tgt and app and return tgt @ app
        char* s = (char*) malloc((strlen(tgt) + strlen(app) + 1) * sizeof(char));
        memcpy(s, tgt, strlen(tgt));
        memcpy(s + strlen(tgt), app, strlen(app));
        s[strlen(tgt) + strlen(app)] = '\0';
        return s;
}

void makeNum(char* str, struct num* n){
        /*  Primary lexing function for generation of a num.
            It checks for starting signs and then decides size
            of the num accordingly. It groups elements in bunches 
            of 9 elements since the radix is 1e9. Then, it generates
            the num in 'n'
        */
        int start = 0, i, j, k;
        if(str[0] == '-'){
                str = trim(str);
                n->sign = -1;
        }
        else if(str[0] == '+'){
                str = trim(str);
                n->sign = +1;
        }
        else{
                n->sign = +1;
        }
    
        int floatIndex = indexOf(str, '.');
        if(floatIndex == -1)
                str = concat(str, ".0");

        floatIndex = indexOf(str, '.');

        int len = strlen(str);

        int left = floatIndex, right = len - floatIndex - 1;
        n->floatIndex = (right - 1)/ 9 + 1;
        int size = ((left - 1) / 9 + (right - 1) / 9 + 2) * sizeof(long);
        n->size = size / sizeof(long);
        n->digits = (long*)malloc(size);

        char* before = calloc(floatIndex + 1, sizeof(char));
        char* after = calloc(len - floatIndex , sizeof(char));
        before = memcpy(before, str, floatIndex);
        before[floatIndex] = '\0';
        after = memcpy(after, str + floatIndex + 1, len - floatIndex - 1);
        after[len - floatIndex -1] = '\0';
        reverse(before);

        char* temp = (char*)calloc(10, sizeof(char));
        for(i = 0, j = 0, k = n->floatIndex ; i < floatIndex; i++){
                if(j == NUM_SIZE){
                        temp[9] = '\0';
                        int l = strlen(temp);
                        n->digits[k] = atoll(temp);
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
		int c;
                for(c = 0; c < 9 - j; c++)
                        remaining[c] = '0';
                remaining[NUM_SIZE] = '\0';
                n->digits[k] = atoll(remaining);
        }

        free(temp);
        temp = (char*)calloc(10, sizeof(char));
        for(i = 0, j = 0, k = n->floatIndex - 1; i < len - floatIndex - 1; i++){
                if(j == NUM_SIZE ){;
                        temp[9] = '\0';
                        n->digits[k] = atoll(temp);
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
		int c;
                for(c = j; c < NUM_SIZE; c++)
                        remaining[c] = '0';
                remaining[NUM_SIZE] = '\0';
                n->digits[k] = atoll(remaining);
        }
}

void shiftLeft(struct num* n, int offSet){
        //Useful for appending zeros in number while adding/subtracting
        
        long* digits = (long*)calloc((n->size + offSet), sizeof(long));
        memcpy(digits + offSet, n->digits, n->size * sizeof(long));
        n->digits = digits;
        n->size += offSet;
        n->floatIndex += offSet;
}

void shiftRight(struct num* n, int offSet){
        //Useful for appending zeros in number while adding/subtracting
        
        long* digits = (long*)calloc((n->size + offSet), sizeof(long));
        memcpy(digits, n->digits, n->size * sizeof(long));
        n->digits = digits;
        n->size += offSet;
}

struct num* subNum(struct num* n1, struct num* n2){
    
        /*  Subtracting two numbers is done using same old school chain carry
            method. This method has complexity of O(n) where n = max(size(n1), size(n2)).
            Initially signs are checked in this manner:
                (-) - (-) -> (-) + (+)
                (-) - (+) -> (-) + (-)
                (+) - (-) -> (+) + (+)
                Subtraction is only carried out when elements are of + sign 
                and the first element is larger than the second. 
                If |n2| > |n1|, we give the answer as -(|n1| - |n2|)
        */
        
        if(n1->sign == -1 && n2->sign == -1){
            struct num* n1_ = (struct num*)malloc(sizeof(struct num));
            struct num* n2_ = (struct num*)malloc(sizeof(struct num));
            memcpy(n1_, n1, sizeof(struct num));
            memcpy(n1_->digits, n1->digits, n1->size);
            memcpy(n2_, n2, sizeof(struct num));
            memcpy(n2_->digits, n1->digits, n1->size);
            n2_->sign = 1;
            n1_->sign = 1;
            return subNum(n2_, n1_);
        }
        if(n1->sign == -1 && n2->sign == 1){
            struct num* n2_ = (struct num*)malloc(sizeof(struct num));
            memcpy(n2_, n2, sizeof(struct num));
            memcpy(n2_->digits, n1->digits, n1->size);
            n2_->sign = -1;
            return addNum(n1, n2_);
        }
        if(n1->sign == 1 && n2->sign == -1){
            struct num* n2_ = (struct num*)malloc(sizeof(struct num));
            memcpy(n2_, n2, sizeof(struct num));
            memcpy(n2_->digits, n1->digits, n1->size);
            n2_->sign = 1;
            return addNum(n1, n2_);
        }
        struct num* sub = (struct num*)(malloc(sizeof(struct num)));
        if(n1->floatIndex < n2->floatIndex)
                shiftLeft(n1, n2->floatIndex - n1->floatIndex);
        else if(n1->floatIndex > n2->floatIndex)
                shiftLeft(n2, n1->floatIndex - n2->floatIndex);
        if(compareMgn(n1, n2) >= 0){
                sub->floatIndex = n1->floatIndex;
                sub->sign = 1;
                sub->size = max(n1->size, n2->size);
                sub->digits = (long*)calloc(sub->size, sizeof(long));
                long a, b, borrow = 0;
		int i = 0;
                for(i = 0; i < sub->size; i++){
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

struct num* addNum(struct num* n1, struct num* n2){
    
        /*  Adding two numbers is done using same old school chain carry
            method. This method has complexity of O(n) where n = max(size(n1), size(n2))
            If signs are different, instead of adding, we subtract the two numbers. 
            In case they are the same, we perform usual addition. 
        */
        
        struct num* add = (struct num*)(malloc(sizeof(struct num)));
    	if(n1->sign != n2->sign) {
    		if(n1->sign == -1) 	
    			return subNum(n2, n1);
    		else 
    			return subNum(n1, n2);
    	}

        if(n1->floatIndex < n2->floatIndex)
                shiftLeft(n1, n2->floatIndex - n1->floatIndex);
        else if(n1->floatIndex > n2->floatIndex)
                shiftLeft(n2, n1->floatIndex - n2->floatIndex);
        add->sign = n1->sign;
        add->floatIndex = n1->floatIndex;
        add->size = max(n1->size, n2->size) + 1;
        add->digits = (long*)calloc(add->size, sizeof(long));
        long a, b, carry = 0;
	int i = 0;
        for(i = 0; i < add->size - 1; i++){
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
        /*  Multiplies the number by RADIX ^ n using appropriate
            right and left shifts
        */
        if(n > 0){
                shiftLeft(n1, n);
                n1->floatIndex -= n;
        }
        else if(n < 0){
                shiftRight(n1, -n);
                n1->floatIndex -= n;
        }
}

int compareMgn(struct num* n1, struct num* n2){
        /*  In case |n1| > |n2|, 1 is returned.
            In case |n1| = |n2|, 0 is returned.
            In case |n1| < |n2|, -1 is returned.
        */
        int s1 = n1->floatIndex, s2 = n2->floatIndex;
        int sign = 0;
        while(s1 < n1->size || s2 < n2->size){
                long l1, l2;
                if(s1 >= n1->size) l1 = 0;
                else l1 = n1->digits[s1];

                if(s2 >=n2->size) l2 = 0;
                else l2 = n2->digits[s2];

                if(sign == 0 && l1 == l2)
                        sign = 0;
                else if(l1 > l2)
                        sign = 1;
                else if(l1 < l2)
                        sign = -1;
                s1++; s2++;
        }

        if(sign == 0){
                s1 = n1->floatIndex - 1, s2 = n2->floatIndex - 1;
                while((s1 >= 0 || s2 >= 0) && (sign == 0)){
                        long l1, l2;
                        if(s1 < 0) l1 = 0;
                        else l1 = n1->digits[s1];

                        if(s2 < 0) l2 = 0;
                        else l2 = n2->digits[s2];

                        if(l1 == l2)
                                sign = 0;
                        else if(l1 > l2)
                                sign = 1;
                        else
                                sign = -1;
                        s1--; s2--;
                }
        }
        return sign;
}

char* toString(struct num* n){
        /*  Converts a num to a string, removing redunancies such
            as leading zeros and zero after the decimal point*/
        if(n == NULL)
            return "Error";
        int size = n->size * NUM_SIZE + 1;
        int end = n->size * NUM_SIZE + 1;
        char* s = (char*)calloc(end + 1, sizeof(char));
        s[end] = '\0';
        int floatIndex;
        end--;
	int i = 0;
        for(i = 0; i < n->size; i++){
                if(i == n->floatIndex){
                        s[end] = '.';
                        floatIndex = end;
                        end--;
                }
                char buffer[10];
                char* str = (char*)calloc(10, sizeof(char));
                snprintf(buffer, 10, "%ld", n->digits[i]);
                int len = strlen(buffer);
                if(len != 9){
                        memcpy(str + 9 - len, buffer, len + 1);
			int k = 0;
                        for(k = 0; k < 9 - len; k++)
                                str[k] = '0';
                }
                else
                        str = buffer;
		int j;
                for(j = 8; j >= 0; j--){
                        s[end] = str[j];
                        end--;
                }
        }
        int start = -1;
        end = -1;
         for(i = 0; i < size && (start == -1); i++){
                if(s[i] != '0'){
                        if(s[i] == '.') 
        				        start = i - 1;                 
        		      	else
        				        start = i;
                }
         }
        for(i = n->size * NUM_SIZE; i >= 0 && (end == -1); i--)
                if(s[i] != '0')
                        end = i;

        if(end == floatIndex){
                char* t = (char*)calloc(end - start + 1, sizeof(char));
                t[end - start] = '\0';
                memcpy(t, s + start, end - start);
                s = t;
        }
        else{

                char* t = (char*)calloc(end - start + 2, sizeof(char));
                t[end - start + 1] = '\0';
                memcpy(t, s + start, end - start + 1);
                s = t;

                int fp = 0;
                int count = 0;
                for(i = 0; i < (end - start + 2); i++){
                        if(s[i] == '.')
                                fp = 1;
                        if(fp)
                                count++;
                        if(count == 22){
                                s[i] = '\0';
                                break;
                        }
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
        //  Multiplies long with num 
        
        mult->sign = n1->sign;
        mult->size = n1->size + 1;
        mult->floatIndex = n1->floatIndex;
        mult->digits = (long*)calloc(n1->size + 1, sizeof(long));
        long carry = 0;
	int i; 
        for(i = 0; i < n1->size + 1; i++){
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
        /*  Multiplied two nums using school method of multiplicatoin
            Complexity is O(n^2) where n is the number of digits in the
            number.
        */
        
        struct num* mult = (struct num*)malloc(sizeof(struct num));
        mult->size = n1->size + n2->size;
        mult->digits = (long*)calloc(mult->size, sizeof(long));
        int same = 1;
        if(n1->sign != n2->sign)
                same = -1;
                
        mult->floatIndex = n1->floatIndex + n2->floatIndex;
	int i = 0;
        for(i = 0; i < n2->size; i++){
                struct num* temp = (struct num*)malloc(sizeof(struct num));
                struct num* acc = (struct num*)malloc(sizeof(struct num));
                multLong(n1, n2->digits[i], temp);
                multRadix(temp, i - n2->floatIndex);
                mult = addNum(mult, temp);
                free(temp);
                free(acc);
        }
        mult->sign = same;
        return mult;
}

int numExponent(struct num* n1){ return n1->size - n1->floatIndex; }

long guess(struct num* n1, struct num* n2){
        //Makes the estimate of the quotient using binary search from 0 to RADIX - 1
        long l = 0, r = RADIX - 1;
        while(r - l != 1){
                long mid = (l + r) / 2;

                char buffer[10];
                snprintf(buffer, 10, "%ld", mid);

                struct num* nmid = (struct num*)malloc(sizeof(struct num));
                makeNum(buffer, nmid);
                struct num* g = multNum(nmid, n1);
                int compare = compareMgn(g, n2);
                if(compare > 0) {
                        r = mid;
                }
                else if(compare == 0){
                        return mid;
                }
                else{
                        l = mid;
                }
                free(nmid);
        }
        return l;
}

struct num * divNum(struct num* n1, struct num* n2){
        /*  Divides using school algorithm. Complexity depends
            on the precision required. In this particular implementation
            30 digits of accuracy has been given. 
        */
        int same = 1;
        if(n1->sign != n2->sign)
            same = -1;
        n1->sign = 1;
        n2->sign = 1;
        int p1 = numExponent(n1), p2 = numExponent(n2);
        struct num* div = (struct num*)malloc(sizeof(struct num));
        struct num* divid = (struct num*)malloc(sizeof(struct num));
        memcpy(div, n1, sizeof(struct num));
        memcpy(div->digits, n1->digits, n1->size);
        memcpy(divid, n2, sizeof(struct num));
        memcpy(divid->digits, n2->digits, n2->size);

        struct num* quo = (struct num*)malloc(sizeof(struct num));
        makeNum("0", quo);
        
        struct num* zero = (struct num*)malloc(sizeof(struct num));
        makeNum("0", zero);

        int power = p2 - p1;
        multRadix(div, power);
        int NUM_ITER = power + 30;
	int i = 0;
        for(i = 0; i < NUM_ITER; i++){
                long g = guess(div, divid);
                char buffer[10];
                snprintf(buffer, 10, "%ld", g);
                struct num* temp = (struct num*)malloc(sizeof(struct num));
                makeNum(buffer, temp);
                divid = subNum(divid, multNum(temp, div));
                multRadix(temp, power - i);
                quo = addNum(quo, temp);
                multRadix(div, -1);
                free(temp);
                if(compareMgn(zero, divid) == 0){
                        quo->sign = same;
                        return quo;
                }
        }
        quo->sign = same;
        return quo;
}

struct num * absolute(struct num* n){
        //Reverses sign in case required.
        struct num* ab = (struct num*)malloc(sizeof(struct num));
        memcpy(ab, n, sizeof(struct num));
        memcpy(ab->digits, n->digits, n->size);
        ab->sign = +1;
        return ab;
}

struct num* sqrtNum(struct num* n){
        /*  Relies on bisection method. 
            The initial estimate is made depending on the number provided.
            If n > 1 then the initial estimate is n because 0 < 1 < sqrt(n) < n
            In n <= t then the initial estimate is 1 because 0 < n < sqrt(n) < 1
        */
        if(n->sign == -1){
            printf("SQRT of negative numbers is not allowed\n");
            return NULL;
        }
        struct num* one = (struct num*)malloc(sizeof(struct num));
        makeNum("1", one);
        struct num* two = (struct num*)malloc(sizeof(struct num));
        makeNum("2", two);

        struct num* l = (struct num*)malloc(sizeof(struct num));
        makeNum("0", l);
        struct num* r = (struct num*)malloc(sizeof(struct num));
        int compare = compareMgn(one, n);
        if(compare < 0){
                memcpy(r, n, sizeof(struct num));
                memcpy(r->digits, n->digits, n->size);
        }
        else if(compare = 0){
                return one;
        }
        else{
                memcpy(r, one, sizeof(struct num));
                memcpy(r->digits, one->digits, one->size);
        }


        int N_ITER = 100;   //Arbitrarily set. 
	int i = 0;
        for(i = 0; i < N_ITER; i++){
                struct num* mid = divNum(two, addNum(l, r));
                struct num* midSq = multNum(mid, mid);

                int compareMid = compareMgn(midSq, n);
                if(compareMid == 0){
                        return mid;
                }
                else if(compareMid < 0){
                        memcpy(l, mid, sizeof(struct num));
                        memcpy(l->digits, mid->digits, mid->size);
                }
                else{
                        memcpy(r, mid, sizeof(struct num));
                        memcpy(r->digits, mid->digits, mid->size);

                }
                free(mid);
                free(midSq);
        }
        return r;
}

struct num* int_power(struct num* n, int p){
        if(p == 0){
                struct num* one = (struct num*)malloc(sizeof(struct num));
                makeNum("1", one);
                return one;
        }

        if(p % 2 == 0){
                struct num* even = int_power(n, p / 2);
                return multNum(even, even);
        }
        else if(p % 2 == 1){
                struct num* even = int_power(n, p / 2);
                return multNum(multNum(even, even), n);
        }
}

struct num* power(struct num* n, double p){
        /*  Power breaks down the power into the fractional and the integer
            part. For the integer part, it computes using the recursion the 
            value to be multiplied. For the fractional part, it looks at the 
            binary representation of the number and takes square root accordingly. 
        */
        
        struct num* one = (struct num*)malloc(sizeof(struct num));
        if(n->sign == -1){
            printf("POWERS of negative numbers is not allowed\n");
            return NULL;
        }
        int MAX_ITER = 5;
        if(p > 0){
                struct num* pow_int = int_power(n, (int)p);
                double frac = p - (int)p;
                if(frac == 0.0){
                        return pow_int;
                }
                struct num* curr = (struct num*)malloc(sizeof(struct num));
                memcpy(curr, n, sizeof(struct num));
                memcpy(curr->digits, n->digits, n->size);
                
                curr = sqrtNum(curr);
		int i = 0;
                for(i = 0; i < MAX_ITER; i++){
                        frac *= 2;
                        if(frac >= 1){
                                pow_int = multNum(pow_int, curr);
                                frac -= 1;
                        }
                        else if(frac == 0)
                                return pow_int;
                        curr = sqrtNum(curr);
                }
                free(curr);
                return pow_int;
        }
        else {
                makeNum("1", one);
                return power(divNum(n, one), -p);
        }
}
void removeNewLine(char* s){
	int len = strlen(s);
	if(s[len - 1] == '\n')
		s[len - 1] = '\0';
}

int main(int argc, char* argv){

        size_t len = 0;
        char* buffer = NULL;
        while(getline(&buffer, &len, stdin) != -1){
                char ** tokens = (char**)malloc(3 * sizeof(char *));
                char * token = strtok(buffer, " ");
                int i = 0;
                while( token != NULL ) {
                        tokens[i] = token;
                        token = strtok(NULL, " ");
                        i++;
                }

                if(strcmp(tokens[0], "ADD") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
                        makeNum(tokens[1], n1);
                        struct num* n2 =  (struct num*)malloc(sizeof(struct num));
			removeNewLine(tokens[2]);                        
			makeNum(tokens[2], n2);
                        printf("%s\n", toString(addNum(n1, n2)));
                        free(n1); free(n2);
                }
                else if(strcmp(tokens[0], "SUB") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
                        makeNum(tokens[1], n1);
                        struct num* n2 =  (struct num*)malloc(sizeof(struct num));
			            removeNewLine(tokens[2]);
                        makeNum(tokens[2], n2);
                        printf("%s\n", toString(subNum(n1, n2)));
                        free(n1); free(n2);
                }
                else if(strcmp(tokens[0], "MUL") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
                        makeNum(tokens[1], n1);
                        struct num* n2 =  (struct num*)malloc(sizeof(struct num));
			            removeNewLine(tokens[2]);
                        makeNum(tokens[2], n2);
                        printf("%s\n", toString(multNum(n1, n2)));
                        free(n1); free(n2);
                }
                else if(strcmp(tokens[0], "DIV") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
                        makeNum(tokens[1], n1);
                        struct num* n2 =  (struct num*)malloc(sizeof(struct num));
			            removeNewLine(tokens[2]);
                        makeNum(tokens[2], n2);
                        printf("%s\n", toString(divNum(n2, n1)));
                        free(n1); free(n2);
                }
                else if(strcmp(tokens[0], "SQRT") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
		            	removeNewLine(tokens[1]);
                        makeNum(tokens[1], n1);
                        printf("%s\n", toString(sqrtNum(n1)));
                        free(n1);
                }
                else if(strcmp(tokens[0], "ABS") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
            			removeNewLine(tokens[1]);
            			makeNum(tokens[1], n1);
                        printf("%s\n", toString(absolute(n1)));
                        free(n1);
                }
                else if(strcmp(tokens[0], "POW") == 0){
                        struct num* n1 =  (struct num*)malloc(sizeof(struct num));
                        makeNum(tokens[1], n1);
                        printf("%s\n", toString(power(n1, atof(tokens[2]))));
                        free(n1);
                }
                else
                        printf("Invalid Operation\n");
        }
	return 0;
}

