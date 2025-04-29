#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_DEGREE 100
#define MAX_INPUT_LENGTH 1000

typedef struct {
    double coefficients[MAX_DEGREE + 1];
    int degree;
} Polynomial;

// Function prototypes
void parsePolynomial(char *input, Polynomial *poly);
void printPolynomial(Polynomial *poly);
void dividePolynomials(Polynomial *dividend, Polynomial *divisor, Polynomial *quotient, Polynomial *remainder);
void multiplyPolynomials(Polynomial *p1, Polynomial *p2, Polynomial *result);
void subtractPolynomials(Polynomial *p1, Polynomial *p2, Polynomial *result);
void initPolynomial(Polynomial *poly);

int main(void) {
    char input[MAX_INPUT_LENGTH];
    Polynomial dividend, divisor, quotient, remainder;
    
    // Initialize polynomials
    initPolynomial(&dividend);
    initPolynomial(&divisor);
    initPolynomial(&quotient);
    initPolynomial(&remainder);
    
    // Get dividend
    printf("Enter the dividend polynomial (e.g., 2x^3-x^2+3): ");
    fgets(input, MAX_INPUT_LENGTH, stdin);
    parsePolynomial(input, &dividend);
    
    // Get divisor
    printf("Enter the divisor polynomial (e.g., x-1): ");
    fgets(input, MAX_INPUT_LENGTH, stdin);
    parsePolynomial(input, &divisor);
    
    // Check if divisor is zero
    if (divisor.degree < 0) {
        printf("Error: Division by zero polynomial is not allowed.\n");
        return 1;
    }
    
    // Perform division
    dividePolynomials(&dividend, &divisor, &quotient, &remainder);
    
    // Display results
    printf("\nDividend: ");
    printPolynomial(&dividend);
    printf("\nDivisor: ");
    printPolynomial(&divisor);
    printf("\nQuotient: ");
    printPolynomial(&quotient);
    printf("\nRemainder: ");
    printPolynomial(&remainder);
    printf("\n");
    
    return 0;
}

void initPolynomial(Polynomial *poly) {
    for (int i = 0; i <= MAX_DEGREE; i++) {
        poly->coefficients[i] = 0.0;
    }
    poly->degree = -1; // Empty polynomial
}

void parsePolynomial(char *input, Polynomial *poly) {
    initPolynomial(poly);
    
    char *token = input;
    char *end;
    int sign = 1;
    double coefficient;
    int exponent;
    int foundX;
    
    while (*token) {
        // Skip whitespace
        while (isspace(*token)) token++;
        
        if (*token == '\0') break;
        
        // Handle sign
        if (*token == '+') {
            sign = 1;
            token++;
            continue;
        } else if (*token == '-') {
            sign = -1;
            token++;
            continue;
        }
        
        // Parse coefficient
        if (*token == 'x' || *token == 'X') {
            coefficient = sign * 1.0;
        } else {
            coefficient = sign * strtod(token, &end);
            token = end;
        }
        
        // Check for x term
        foundX = 0;
        if (*token == 'x' || *token == 'X') {
            foundX = 1;
            token++;
            
            // Check for exponent
            if (*token == '^') {
                token++;
                exponent = (int)strtol(token, &end, 10);
                token = end;
            } else {
                exponent = 1;
            }
        } else {
            exponent = 0;
        }
        
        // If we have no x, but we have a coefficient, it's the constant term
        if (!foundX) {
            exponent = 0;
        }
        
        // Update polynomial
        poly->coefficients[exponent] += coefficient;
        if (exponent > poly->degree) {
            poly->degree = exponent;
        }
    }
    
    // Recalculate degree (in case highest terms cancelled out)
    for (int i = poly->degree; i >= 0; i--) {
        if (fabs(poly->coefficients[i]) > 1e-10) {
            poly->degree = i;
            break;
        }
        if (i == 0) poly->degree = -1; // Zero polynomial
    }
}

void printPolynomial(Polynomial *poly) {
    int first = 1;
    
    if (poly->degree < 0) {
        printf("0");
        return;
    }
    
    for (int i = poly->degree; i >= 0; i--) {
        double coef = poly->coefficients[i];
        
        if (fabs(coef) < 1e-10) continue; // Skip zero coefficients
        
        if (coef > 0 && !first) {
            printf(" + ");
        } else if (coef < 0) {
            printf(first ? "-" : " - ");
            coef = -coef;
        }
        
        // Print coefficient if it's not 1 or if it's the constant term
        if (fabs(coef - 1.0) > 1e-10 || i == 0) {
            // Print as integer if it's very close to an integer
            if (fabs(coef - round(coef)) < 1e-10) {
                printf("%d", (int)round(coef));
            } else {
                printf("%.2f", coef);
            }
        }
        
        // Print variable and exponent
        if (i > 0) {
            printf("x");
            if (i > 1) {
                printf("^%d", i);
            }
        }
        
        first = 0;
    }
    
    if (first) { // No terms were printed, polynomial is zero
        printf("0");
    }
}

void dividePolynomials(Polynomial *dividend, Polynomial *divisor, Polynomial *quotient, Polynomial *remainder) {
    initPolynomial(quotient);
    initPolynomial(remainder);
    
    // Copy dividend to remainder
    remainder->degree = dividend->degree;
    for (int i = 0; i <= dividend->degree; i++) {
        remainder->coefficients[i] = dividend->coefficients[i];
    }
    
    // If degree of dividend is less than degree of divisor, quotient is zero and remainder is dividend
    if (dividend->degree < divisor->degree) {
        return;
    }
    
    // Perform polynomial long division
    quotient->degree = dividend->degree - divisor->degree;
    
    for (int i = dividend->degree; i >= divisor->degree; i--) {
        // Get the current coefficient to work with
        double coef = remainder->coefficients[i];
        
        // Skip if coefficient is zero (or very close to zero)
        if (fabs(coef) < 1e-10) continue;
        
        // Calculate quotient term
        int qExponent = i - divisor->degree;
        double qCoef = coef / divisor->coefficients[divisor->degree];
        quotient->coefficients[qExponent] = qCoef;
        
        // Subtract (divisor * quotient term) from remainder
        for (int j = 0; j <= divisor->degree; j++) {
            remainder->coefficients[j + qExponent] -= qCoef * divisor->coefficients[j];
        }
    }
    
    // Recalculate the degree of the remainder
    remainder->degree = divisor->degree - 1;
    while (remainder->degree >= 0 && fabs(remainder->coefficients[remainder->degree]) < 1e-10) {
        remainder->degree--;
    }
}

void multiplyPolynomials(Polynomial *p1, Polynomial *p2, Polynomial *result) {
    initPolynomial(result);
    
    result->degree = p1->degree + p2->degree;
    
    for (int i = 0; i <= p1->degree; i++) {
        for (int j = 0; j <= p2->degree; j++) {
            result->coefficients[i + j] += p1->coefficients[i] * p2->coefficients[j];
        }
    }
}

void subtractPolynomials(Polynomial *p1, Polynomial *p2, Polynomial *result) {
    initPolynomial(result);
    
    result->degree = (p1->degree > p2->degree) ? p1->degree : p2->degree;
    
    for (int i = 0; i <= p1->degree; i++) {
        result->coefficients[i] += p1->coefficients[i];
    }
    
    for (int i = 0; i <= p2->degree; i++) {
        result->coefficients[i] -= p2->coefficients[i];
    }
    
    // Recalculate the degree in case of cancellations
    while (result->degree >= 0 && fabs(result->coefficients[result->degree]) < 1e-10) {
        result->degree--;
    }
}
