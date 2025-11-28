/*
    Практическая работа номер 6
    Помощник вычислений задания из таблицы
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <cairo.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Debugging colors definition
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[36m"
#define COLOR_GRAY    "\033[90m"
#define COLOR_RESET   "\033[0m"
/* 
    Examples:
    printf(COLOR_GREEN "✅ Успех: операция завершена\n" COLOR_RESET);
    printf(COLOR_YELLOW "⚠️  Предупреждение: что-то пошло не так\n" COLOR_RESET);
    printf(COLOR_RED "❌ Ошибка: критическая ошибка!\n" COLOR_RESET);
    printf(COLOR_BLUE "ℹ️  Информация: полезная информация\n" COLOR_RESET);
    printf(COLOR_GRAY "🔧 Отладка: debug информация\n" COLOR_RESET);
*/


struct CorrelationTable {
    size_t xNum;
    size_t yNum;
    double *xValue;
    double *yValue;
    double *mx;
    double *my;
    double **freq;
    size_t total;
};

struct RegressionResults {
    long double sum_weighted_x;
    long double sum_weighted_y;
    long double sum_weighted_xy;
    long double sum_weighted_x2;
    long double sum_weighted_y2;
    long double x_bar;
    long double y_bar;
    long double variance_x;
    long double variance_y;
    long double std_dev_x;
    long double std_dev_y;
    long double covariance_xy;
    long double correlation_xy;
    long double slope;
    long double intercept;
};

void freeTable(struct CorrelationTable *table) {
    if (table->xValue) {
        free(table->xValue);
        table->xValue = NULL;
    }
    if (table->yValue) {
        free(table->yValue);
        table->yValue = NULL;
    }
    if (table->mx) {
        free(table->mx);
        table->mx = NULL;
    }
    if (table->my) {
        free(table->my);
        table->my = NULL;
    }
    if (table->freq) {
        for (size_t i = 0; i < table->xNum; i++) {
            if (table->freq[i]) {
                free(table->freq[i]);
                table->freq[i] = NULL;
            }
        }
        free(table->freq);
        table->freq = NULL;
    }
    
    table->xNum = 0;
    table->yNum = 0;
    table->total = 0;
}

double array_sum(const double *array, size_t size) {
    double sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum = sum + array[i];
    }
    return sum;
}

bool inputTable(struct CorrelationTable *table) {
    // input operations for CorrelationTable
    printf("Ввод значений в таблицу\n");
    
    printf("Введите количество значений X: ");
    scanf("%zu", &table->xNum);
    
    printf("Введите количество значений Y: ");
    scanf("%zu", &table->yNum);

    table->xValue = malloc(table->xNum * sizeof(double));
    table->yValue = malloc(table->yNum * sizeof(double));
    table->mx = malloc(table->xNum * sizeof(double));
    table->my = malloc(table->yNum * sizeof(double));
    table->freq = malloc(table->xNum * sizeof(double*));

    if (!table->xValue || !table->yValue || !table->mx || !table->my || !table->freq) {
        freeTable(table);
        printf("\n" COLOR_RED "❌  ERROR: Can't allocate enough memory" COLOR_RESET "\n");
        return false;
    }

    for (size_t i = 0; i < table->xNum; i++) {
        table->freq[i] = malloc(table->yNum * sizeof(double));
        if (!table->freq[i]) {
            printf("\n" COLOR_RED "❌  ERROR: Can't allocate memory for freq[%zu]" COLOR_RESET "\n", i);
            freeTable(table);
            return false;
        }
    }
    
    printf("Введите X столбец: \n");
    for (size_t i = 0; i < table->xNum; i++) {
        printf("X[%zu] = ", i);
        scanf("%lf", &table->xValue[i]);
    }
    
    printf("Введите Y строка: \n");
    for (size_t i = 0; i < table->yNum; i++) {
        printf("Y[%zu] = ", i);
        scanf("%lf", &table->yValue[i]);
    }

    printf("Введите mx столбец: \n");
    for (size_t i = 0; i < table->xNum; i++) {
        printf("Mx[%zu] = ", i);
        scanf("%lf", &table->mx[i]);
    }
    
    printf("Введите my строка: \n");
    for (size_t i = 0; i < table->yNum; i++) {
        printf("My[%zu] = ", i);
        scanf("%lf", &table->my[i]);
    }

    printf("Введите массив значений построчно: \n");
    for (size_t i = 0; i < table->xNum; i++) {
        for (size_t j = 0; j < table->yNum; j++) {
            printf("X[%zu]=%.4lf Y[%zu]=%.4lf = ", i, table->xValue[i], j, table->yValue[j]);
            scanf("%lf", &table->freq[i][j]);
        }
        puts("");
    }

    printf("Введите общее количество наблюдений: ");
    scanf("%zu", &table->total);
    return true;
}

void initMyTable(struct CorrelationTable *table) {
    // Очищаем структуру
    memset(table, 0, sizeof(struct CorrelationTable));
    
    table->xNum = 6;
    table->yNum = 8;
    table->total = 100;
    
    table->xValue = malloc(table->xNum * sizeof(double));
    table->yValue = malloc(table->yNum * sizeof(double));
    table->mx = malloc(table->xNum * sizeof(double));
    table->my = malloc(table->yNum * sizeof(double));
    table->freq = malloc(table->xNum * sizeof(double*));
    
    for (size_t i = 0; i < table->xNum; i++) {
        table->freq[i] = malloc(table->yNum * sizeof(double));
    }
    
    if (!table->xValue || !table->yValue || !table->mx || !table->my || !table->freq) {
        printf(COLOR_RED "❌ Ошибка выделения памяти\n" COLOR_RESET);
        freeTable(table);
        return;
    }
    
    double x_values[] = {200, 360, 520, 680, 840, 1000};
    for (size_t i = 0; i < table->xNum; i++) {
        table->xValue[i] = x_values[i];
    }
    
    double y_values[] = {2.2, 3.6, 5.0, 6.4, 7.8, 9.2, 10.6, 12.0};
    for (size_t j = 0; j < table->yNum; j++) {
        table->yValue[j] = y_values[j];
    }
    
    double mx_values[] = {12, 15, 33, 21, 7, 12};
    for (size_t i = 0; i < table->xNum; i++) {
        table->mx[i] = mx_values[i];
    }
    
    double my_values[] = {5, 10, 21, 18, 23, 9, 8, 6};
    for (size_t j = 0; j < table->yNum; j++) {
        table->my[j] = my_values[j];
    }
    
    int frequencies[6][8] = {
        {5, 3, 4, 0, 0, 0, 0, 0},
        {0, 7, 8, 0, 0, 0, 0, 0},
        {0, 0, 9, 10, 14, 0, 0, 0},
        {0, 0, 0, 8, 7, 6, 0, 0},
        {0, 0, 0, 0, 2, 3, 2, 0},
        {0, 0, 0, 0, 0, 0, 6, 6}
    };
    
    for (size_t i = 0; i < table->xNum; i++) {
        for (size_t j = 0; j < table->yNum; j++) {
            table->freq[i][j] = frequencies[i][j];
        }
    }
    
    printf("\n" COLOR_GREEN "✅ Таблица успешно инициализирована автоматически\n" COLOR_RESET);
}

bool checkTotal(const struct CorrelationTable *table) {
    // Checking the quantity of total and mx my
    double sum_mx = array_sum(table->mx, table->xNum);
    double sum_my = array_sum(table->my, table->yNum);
    if (sum_mx != (double)table->total || sum_my != (double)table->total) {
        printf("\n\n" COLOR_RED "❌  ERROR: Total doesn't match sum of Mx column or My row" COLOR_RESET "\n");
        return false;
    } else {
        printf("\n\n" COLOR_BLUE "ℹ️  INFO: Total matchs sum of Mx column and My row" COLOR_RESET "\n");
        return true;
    }
}

double sumMxX(const struct CorrelationTable *table) {
    double sum = 0;
    for (size_t row = 0; row < table->xNum; row++) {
        sum = sum + (table->mx[row] * table->xValue[row]);
    }
    return sum;
}

double sumMyY(const struct CorrelationTable *table) {
    double sum = 0;
    for (size_t column = 0; column < table->yNum; column++) {
        sum = sum + (table->my[column] * table->yValue[column]);
    }
    return sum;
}

double sumXY(const struct CorrelationTable *table) {
    double sumXY = 0;
    for (size_t row = 0; row < table->xNum; row++) {
        double tmp_sum = 0;
        for (size_t column = 0; column < table->yNum; column++) {
            tmp_sum = tmp_sum + (table->freq[row][column] * table->yValue[column]);
        }
        sumXY = sumXY + (tmp_sum * table->xValue[row]);
    }

    return sumXY;
}

double xBar(const struct CorrelationTable *table) {
    return (sumMxX(table) / (double)table->total);
}

double yBar(const struct CorrelationTable *table) {
    return (sumMyY(table) / (double)table->total);
}

double sumMxX2(const struct CorrelationTable *table) {
    double sum = 0;
    for (size_t row = 0; row < table->xNum; row++) {
        sum = sum + (table->mx[row] * table->xValue[row] * table->xValue[row]);
    }
    return sum;
}

double sumMyY2(const struct CorrelationTable *table) {
    double sum = 0;
    for (size_t column = 0; column < table->yNum; column++) {
        sum = sum + (table->my[column] * table->yValue[column] * table->yValue[column]);
    }
    return sum;
}

double varianceX(const struct CorrelationTable *table) {
    double sum_weighted_x = sumMxX(table);
    double sum_weighted_x2 = sumMxX2(table);
    return ( 1.0 / ((double)table->total - 1) ) * ( sum_weighted_x2 - (sum_weighted_x * sum_weighted_x / (double)table->total ));
}

double varianceY(const struct CorrelationTable *table) {
    double sum_weighted_y = sumMyY(table);
    double sum_weighted_y2 = sumMyY2(table);
    return ( 1.0 / ((double)table->total - 1) ) * ( sum_weighted_y2 - (sum_weighted_y * sum_weighted_y / (double)table->total ));
}

void calculateAllResults(const struct CorrelationTable *table, struct RegressionResults *results) {
    // Sum(m_x_i * x_i)
    results->sum_weighted_x = sumMxX(table);
    // Sum(m_y_j * y_j)
    results->sum_weighted_y = sumMyY(table);
    // Sum(x_i * (Sum(freq_ij * y_j)) ) == Sum(y_j * (Sum(freq_ij * x_i)) )
    results->sum_weighted_xy = sumXY(table);

    // \bar{X}, \bar{Y}
    results->x_bar = xBar(table);
    results->y_bar = yBar(table);

    // Sum(m_x_i * x_i * x_i), Sum(m_y_j * y_j * y_j)
    results->sum_weighted_x2 = sumMxX2(table);
    results->sum_weighted_y2 = sumMyY2(table);

    // S_x^2, S_y^2 --- Variance --- D(X), D(Y)
    results->variance_x = varianceX(table);
    results->variance_y = varianceY(table);

    // S_x, S_y --- Standard Deviation --- \sigma(X), \sigma(Y)
    results->std_dev_x = sqrt(results->variance_x);
    results->std_dev_y = sqrt(results->variance_y);

    // Covariance(S_xy) and Correlation(R_xy)
    results->covariance_xy = (1.0 / ((double)table->total - 1)) * (results->sum_weighted_xy - (results->sum_weighted_x * results->sum_weighted_y / (double)table->total));
    results->correlation_xy = results->covariance_xy / (results->std_dev_x * results->std_dev_y);

    /* 
        Empirical regression line --- y = \bar{Y} + Correlation(R_xy) * (Std_dev_y / Std_dev_x) * (k - \bar{x}), k - coefficient
        y = a*x + b
        Slope = a
        Intercept = b
    */
   results->slope = results->correlation_xy * (results->std_dev_y / results->std_dev_x);
   results->intercept = results->y_bar - results->slope * results->x_bar;
}

void printResults(const struct RegressionResults *results) {
    printf(COLOR_GREEN "\nРЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЙ:" COLOR_RESET "\n");
    printf(COLOR_GREEN "Основные суммы:\n" COLOR_RESET);
    printf(COLOR_GREEN "\t∑(m_x * x)    = %.2Lf\n", results->sum_weighted_x);
    printf("\t∑(m_y * y)    = %.2Lf\n", results->sum_weighted_y);
    printf("\t∑(m_xy * x*y) = %.2Lf\n", results->sum_weighted_xy);
    printf("\t∑(m_x * x²)   = %.2Lf\n", results->sum_weighted_x2);
    printf("\t∑(m_y * y²)   = %.2Lf\n" COLOR_RESET, results->sum_weighted_y2);
    
    printf(COLOR_GREEN "\n Выборочные характеристики:\n" COLOR_RESET);
    printf(COLOR_GREEN "\tx̄ = %.4Lf, ȳ = %.4Lf\n", results->x_bar, results->y_bar);
    printf("\ts_x² = %.4Lf, s_y² = %.4Lf\n", results->variance_x, results->variance_y);
    printf("\ts_x = %.4Lf, s_y = %.4Lf\n", results->std_dev_x, results->std_dev_y);
    printf("\ts_xy = %.4Lf, r_xy = %.4Lf\n" COLOR_RESET, results->covariance_xy, results->correlation_xy);
    
    printf(COLOR_GREEN "\n УРАВНЕНИЕ РЕГРЕССИИ:" COLOR_RESET "\n");
    printf(COLOR_GREEN"\ty = %.4Lf + %.4Lf * x\n", results->intercept, results->slope);
    printf("\tSlope = %.4Lf, Intercept = %.4Lf\n" COLOR_RESET, results->slope, results->intercept);
}

double to_plot_x(double x, double plot_x1, double x_min, double scale_x) {
    return plot_x1 + (x - x_min) * scale_x;
}

double to_plot_y(double y, double plot_y2, double y_min, double scale_y) {
    return plot_y2 - (y - y_min) * scale_y;
}

void createRegressionPlot(const struct CorrelationTable *table, const struct RegressionResults *results) {
    // Size of image
    int width = 800, height = 600;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);
    
    // Fill background with white color
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    // Searching for min and max X/Y values
    double x_min = table->xValue[0], x_max = table->xValue[0];
    double y_min = table->yValue[0], y_max = table->yValue[0];
    
    for (size_t i = 1; i < table->xNum; i++) {
        if (table->xValue[i] < x_min) x_min = table->xValue[i];
        if (table->xValue[i] > x_max) x_max = table->xValue[i];
    }
    
    for (size_t j = 1; j < table->yNum; j++) {
        if (table->yValue[j] < y_min) y_min = table->yValue[j];
        if (table->yValue[j] > y_max) y_max = table->yValue[j];
    }

    // Find step in graphs
    // X steps - calculate average distance between X values
    double x_step = 0;
    for (size_t i = 0; i < (table->xNum - 1); i++) {
        x_step += (table->xValue[i + 1] - table->xValue[i]);
    }
    x_step /= (table->xNum - 1);
    
    // Y steps - calculate average distance between Y values
    double y_step = 0;
    for (size_t i = 0; i < (table->yNum - 1); i++) {
        y_step += (table->yValue[i + 1] - table->yValue[i]);
    }
    y_step /= (table->yNum - 1);

    // Find X range and Y range 
    double x_range = x_max - x_min;
    double y_range = y_max - y_min;
    
    // Add margins for both sides of graph (10% of range)
    x_min -= x_range * 0.1;
    x_max += x_range * 0.1;
    y_min -= y_range * 0.1;
    y_max += y_range * 0.1;

    // Add margins for graph (space for labels and titles)
    double margin_left = 80, margin_right = 60;
    double margin_top = 60, margin_bottom = 80;
    
    // Define the plotting area within the margins
    double plot_x1 = margin_left, plot_y1 = margin_top;
    double plot_x2 = width - margin_right, plot_y2 = height - margin_bottom;
    
    // Calculate plotting area dimensions
    double plot_width = plot_x2 - plot_x1;
    double plot_height = plot_y2 - plot_y1;
    
    // Calculate scaling factors to convert data coordinates to pixel coordinates
    double scale_x = plot_width / (x_max - x_min);
    double scale_y = plot_height / (y_max - y_min);
    
    // Draw grid - light gray color
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_set_line_width(cr, 0.5);
    
    // Vertical grid lines (X)
    for (double x = ceil(x_min / x_step) * x_step; x <= x_max; x += x_step) {
        double plot_x = to_plot_x(x, plot_x1, x_min, scale_x);
        cairo_move_to(cr, plot_x, plot_y1);
        cairo_line_to(cr, plot_x, plot_y2);
        cairo_stroke(cr);
    }
    
    // Horizontal grid lines (Y)
    for (double y = ceil(y_min / y_step) * y_step; y <= y_max; y += y_step) {
        double plot_y = to_plot_y(y, plot_y2, y_min, scale_y);
        cairo_move_to(cr, plot_x1, plot_y);
        cairo_line_to(cr, plot_x2, plot_y);
        cairo_stroke(cr);
    }
    
    // Draw coordinate axes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    
    // X axis - horizontal line
    cairo_move_to(cr, plot_x1, plot_y2);
    cairo_line_to(cr, plot_x2, plot_y2);
    
    // Y axis - vertical line
    cairo_move_to(cr, plot_x1, plot_y1);
    cairo_line_to(cr, plot_x1, plot_y2);
    cairo_stroke(cr);
    
    // X axis labels
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 0, 0, 0);
    
    for (double x = ceil(x_min / x_step) * x_step; x <= x_max; x += x_step) {
        double plot_x = to_plot_x(x, plot_x1, x_min, scale_x);
        char label[20];
        snprintf(label, sizeof(label), "%.0f", x);
        cairo_move_to(cr, plot_x - 10, plot_y2 + 20);
        cairo_show_text(cr, label);
    }
    
    // Y axis labels
    for (double y = ceil(y_min / y_step) * y_step; y <= y_max; y += y_step) {
        double plot_y = to_plot_y(y, plot_y2, y_min, scale_y);
        char label[20];
        snprintf(label, sizeof(label), "%.1f", y);
        cairo_move_to(cr, plot_x1 - 40, plot_y + 5);
        cairo_show_text(cr, label);
    }
    
    // Axis titles
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, plot_x2 - 20, plot_y2 + 40);
    cairo_show_text(cr, "X");
    cairo_move_to(cr, plot_x1 - 50, plot_y1 - 20);
    cairo_show_text(cr, "Y");
    
    // DATA POINTS - blue points
    cairo_set_source_rgb(cr, 0, 0.4, 0.8);
    for (size_t i = 0; i < table->xNum; i++) {
        for (size_t j = 0; j < table->yNum; j++) {
            if (table->freq[i][j] > 0) {
                double screen_x = to_plot_x(table->xValue[i], plot_x1, x_min, scale_x);
                double screen_y = to_plot_y(table->yValue[j], plot_y2, y_min, scale_y);
                
                // Draw points with size proportional to frequency
                double radius = 3 + table->freq[i][j] * 0.8;
                cairo_arc(cr, screen_x, screen_y, radius, 0, 2 * M_PI);
                cairo_fill(cr);
            }
        }
    }
    
    // EMPIRICAL REGRESSION LINE - red line
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 3);
    
    // Calculate points for regression line
    double start_x = x_min;
    double start_y = results->slope * start_x + results->intercept;
    double end_x = x_max;
    double end_y = results->slope * end_x + results->intercept;
    
    cairo_move_to(cr, to_plot_x(start_x, plot_x1, x_min, scale_x), to_plot_y(start_y, plot_y2, y_min, scale_y));
    cairo_line_to(cr, to_plot_x(end_x, plot_x1, x_min, scale_x), to_plot_y(end_y, plot_y2, y_min, scale_y));
    cairo_stroke(cr);
    
    // Title and equation
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, width/2 - 120, 30);
    cairo_show_text(cr, "Empirical Regression Line. Myasnikov Artem");
    
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, plot_x1, plot_y1 - 20);
    char equation[100];
    snprintf(equation, sizeof(equation), "y = %.4Lf + %.4Lf * x", results->intercept, results->slope);
    cairo_show_text(cr, equation);
    
    // Save to file
    cairo_surface_write_to_png(surface, "regression_plot.png");
    
    // Cleanup resources
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    
    printf(COLOR_GREEN "Graph with empirical regression line saved as 'regression_plot.png'\n" COLOR_RESET);
}

int main(void) {
    struct CorrelationTable table = {0};
    struct RegressionResults results;

    int choice;
    printf("Выберите способ ввода таблицы:\n");
    printf("1 - Ввод таблицы вручную\n");
    printf("2 - Использовать таблицу для первого варианта практической работы номер 6\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            if (!inputTable(&table)) {
                return 1;
            }
            break;
        case 2:
            initMyTable(&table);
            break;
        default:
            printf(COLOR_RED "❌  ERROR: неверный выбор! Допустимые значения: 1 или 2\n" COLOR_RESET);
            return 1;
    }

    if (!checkTotal(&table)) {
        freeTable(&table);
        return 1;
    }

    calculateAllResults(&table, &results);
    printResults(&results);
    createRegressionPlot(&table, &results);
    puts("\n\n" COLOR_GREEN "Made by Artemyasnik!" COLOR_RESET);
    freeTable(&table);
    return 0;
}
