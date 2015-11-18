/*
 * simulate.h
 */

#pragma once

double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array);
void calc_row(int length, double *old, double *cur, double *new);
void calc_edges(int length, double *old, double *cur, double *new);
