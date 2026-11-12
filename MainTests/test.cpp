// Copyright 2026 Malakhov Roman
#include <stdexcept>

#include "MyVector.h"
#include "TTriangleMatrix.h"
#include "Tmathmatrix.h"
#include "Tmathvector.h"
#include "memdata.h"
#include "pch.h"

// Если у вас A[i, j] (C++23), замените на  #define AT(m, i, j) (m[i, j])

// ---------- MemData ----------

// Перемещение: источник обнулён (иначе double free)
TEST(MemData, MoveEmptiesSource) {
  MemData<int> a{1, 2, 3};
  MemData<int> b(static_cast<MemData<int>&&>(a));
  EXPECT_EQ(b.size(), 3u);
  EXPECT_EQ(b[2], 3);
  EXPECT_EQ(a.size(), 0u);
  EXPECT_TRUE(a.data() == nullptr);
}

// Копия независима от оригинала
TEST(MemData, CopyIsDeep) {
  MemData<int> a{1, 2, 3};
  MemData<int> b(a);
  b[0] = 100;
  EXPECT_EQ(a[0], 1);
}

// ---------- Vector ----------

// Кольцо: push_front уводит начало в конец буфера, затем рост должен сохранить
// порядок
TEST(Vector, RingWrapAndGrow) {
  Vector<int> v;
  for (int i = 0; i < 5; i++) v.push_front(i);  // 4 3 2 1 0
  for (int i = 0; i < 50; i++)
    v.push_back(100 + i);  // рост при «закольцованных» данных
  EXPECT_EQ(v.size(), 55u);
  EXPECT_EQ(v[0], 4);
  EXPECT_EQ(v[4], 0);
  EXPECT_EQ(v[5], 100);
  EXPECT_EQ(v.back(), 149);
}

// pop_back/back через границу буфера
TEST(Vector, BackAcrossWrap) {
  Vector<int> v;
  for (int i = 0; i < 8; i++) v.push_front(i);  // 7 6 5 4 3 2 1 0
  for (int i = 0; i < 8; i++) {
    EXPECT_EQ(v.back(), i);
    v.pop_back();
  }
  EXPECT_TRUE(v.is_empty());
}

// Операции на пустом векторе безопасны
TEST(Vector, EmptyIsSafe) {
  Vector<int> v;
  v.pop_front();
  v.pop_back();
  v.erase(0);
  v.insert(5, 3);  // индекс вне диапазона игнорируется
  EXPECT_TRUE(v.is_empty());
}

// insert / erase в начало, середину, конец
TEST(Vector, InsertErase) {
  Vector<int> v{1, 2, 3};
  v.insert(0, 0);
  v.insert(99, 2);
  v.insert(7, v.size());
  EXPECT_EQ(v.size(), 6u);
  EXPECT_EQ(v[0], 0);
  EXPECT_EQ(v[2], 99);
  EXPECT_EQ(v[5], 7);
  v.erase(0);
  v.erase(1);
  v.erase(v.size() - 1);
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);
}

// shrink_to_fit: вектор становится полным, и дальнейшие push_* / back / pop_*
// не ломаются
TEST(Vector, ShrinkThenUse) {
  Vector<int> v;
  for (int i = 0; i < 100; i++) v.push_back(i);
  for (int i = 0; i < 90; i++) v.pop_back();
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), v.size());
  EXPECT_EQ(v.back(), 9);
  v.push_back(10);
  v.push_front(-1);
  EXPECT_EQ(v.size(), 12u);
  EXPECT_EQ(v[0], -1);
  EXPECT_EQ(v[11], 10);
  v.pop_back();
  EXPECT_EQ(v.back(), 9);
}

// shrink_to_fit на пустом векторе (capacity = 0) и последующее использование
TEST(Vector, ShrinkEmptyThenPush) {
  Vector<int> v;
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 0u);
  v.push_back(1);
  v.push_front(0);
  EXPECT_EQ(v[0], 0);
  EXPECT_EQ(v[1], 1);
}

// Перемещённый вектор остаётся пригодным к работе
TEST(Vector, MovedFromIsUsable) {
  Vector<int> a{1, 2, 3};
  Vector<int> b(static_cast<Vector<int>&&>(a));
  EXPECT_EQ(b.size(), 3u);
  EXPECT_TRUE(a.is_empty());
  a.push_back(7);
  EXPECT_EQ(a[0], 7);
}

// ---------- TMathVector ----------

TEST(TMathVector, Arithmetic) {
  TMathVector<int> a{1, 2, 3};
  TMathVector<int> b{10, 20, 30};
  EXPECT_TRUE(a + b == (TMathVector<int>{11, 22, 33}));
  EXPECT_TRUE(b - a == (TMathVector<int>{9, 18, 27}));
  EXPECT_TRUE(a * 2 == (TMathVector<int>{2, 4, 6}));
  EXPECT_EQ(a * b, 140);
}

// Разные размеры -> исключение
TEST(TMathVector, SizeMismatchThrows) {
  TMathVector<int> a{1, 2, 3};
  TMathVector<int> b{1, 2};
  bool add = false, dot = false;
  try {
    (void)(a + b);
  } catch (const std::invalid_argument&) {
    add = true;
  }
  try {
    (void)(a * b);
  } catch (const std::invalid_argument&) {
    dot = true;
  }
  EXPECT_TRUE(add);
  EXPECT_TRUE(dot);
}

// ---------- TMatrix ----------

TEST(TMatrix, ProductAndSum) {
  TMatrix<int> a({{1, 2}, {3, 4}});
  TMatrix<int> b({{5, 6}, {7, 8}});
  TMatrix<int> p = a * b;
  EXPECT_EQ(p[0][0], 19);
  EXPECT_EQ(p[0][1], 22);
  EXPECT_EQ(p[1][0], 43);
  EXPECT_EQ(p[1][1], 50);
  EXPECT_EQ((a + b)[1][1], 12);
}

// Неквадратная матрица и разные размеры -> исключение
TEST(TMatrix, InvalidSizesThrow) {
  bool bad = false, mul = false;
  try {
    TMatrix<int> m({{1, 2, 3}, {4, 5, 6}});
  } catch (const std::invalid_argument&) {
    bad = true;
  }
  try {
    (void)(TMatrix<int>(2) * TMatrix<int>(3));
  } catch (const std::invalid_argument&) {
    mul = true;
  }
  EXPECT_TRUE(bad);
  EXPECT_TRUE(mul);
}

// ---------- TTriangleMatrix ----------

// Чтение/запись над диагональю, запись под диагональю игнорируется
TEST(TTriangleMatrix, BelowDiagonal) {
  TTriangleMatrix<int> a({{3, 4, 3},
                          {9, 4, 3},
                          {9, 9, 3}});  // числа 9 под диагональю отбрасываются
  EXPECT_EQ(a(0, 2), 3);
  EXPECT_EQ(a(1, 0), 0);
  a(2, 1) = 5;
  EXPECT_EQ(a(2, 1), 0);
  a(1, 2) = -6;
  EXPECT_EQ(a(1, 2), -6);
  EXPECT_EQ(a(1, 1), 4);  // соседи не затронуты
}

// Произведение сверено с обычной матрицей
TEST(TTriangleMatrix, ProductMatchesFull) {
  TTriangleMatrix<int> a({{1, 2, 3}, {0, 4, 5}, {0, 0, 6}});
  TTriangleMatrix<int> b({{7, 8, 9}, {0, 1, 2}, {0, 0, 3}});
  TMatrix<int> fa({{1, 2, 3}, {0, 4, 5}, {0, 0, 6}});
  TMatrix<int> fb({{7, 8, 9}, {0, 1, 2}, {0, 0, 3}});
  TTriangleMatrix<int> p = a * b;
  TMatrix<int> fp = fa * fb;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) EXPECT_EQ(p(i, j), fp[i][j]);
}

// Индекс вне матрицы -> исключение
TEST(TTriangleMatrix, OutOfRangeThrows) {
  TTriangleMatrix<int> a(3);
  bool thrown = false;
  try {
    a(3, 0);
  } catch (const std::out_of_range&) {
    thrown = true;
  }
  EXPECT_TRUE(thrown);
}