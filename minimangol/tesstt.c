char *preprocess_input(char *input) {
    char *result = malloc(strlen(input) * 2 + 1); // Worst case: every char needs space
    int j = 0;
    int in_quote = 0;

    for (int i = 0; input[i]; i++) {
        // Handle quoted content
        if (input[i] == '\'' || input[i] == '"') {
            in_quote ^= 1; // Toggle quote state
            result[j++] = ' ';
            result[j++] = input[i];
            result[j++] = ' ';
            continue;
        }

        // Handle operators (| > < &)
        if (!in_quote && strchr("|><&", input[i])) {
            // Add space before operator
            if (j > 0 && result[j-1] != ' ') result[j++] = ' ';
            
            result[j++] = input[i];
            
            // Check for multi-character operators (>>, <<, ||, &&)
            if (i+1 < strlen(input) && (input[i] == input[i+1] || 
                (input[i] == '>' && input[i+1] == '>') || 
                (input[i] == '<' && input[i+1] == '<'))) {
                result[j++] = input[++i];
            }
            
            // Add space after operator
            result[j++] = ' ';
            continue;
        }

        result[j++] = input[i];
    }
    result[j] = '\0';
    return result;
}