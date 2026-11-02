 #include "DS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 1
#define BLACK 0

/*
  Хоёр оюутны мэдээллийн хооронд нь харьцуулах функц
*/
int less(const Student *a, const Student *b)
{
        return strcmp(a->id, b->id) < 0;
}

/* NULL биш бөгөөд улаан өнгөтэй эсэх */
static int isRed(Elm *p)
{
        if (p == NULL)
                return 0;

        return p->color == RED;
}

/* Шинэ орой үүсгэх */
static Elm *newNode(const Student *px)
{
        Elm *p = (Elm *)malloc(sizeof(Elm));

        if (p == NULL)
                exit(1);

        p->x = *px;
        p->color = RED;
        p->L = NULL;
        p->R = NULL;

        return p;
}

/* Зүүн эргүүлэх */
static Elm *rotateLeft(Elm *h)
{
        Elm *x = h->R;

        h->R = x->L;
        x->L = h;

        x->color = h->color;
        h->color = RED;

        return x;
}

/* Баруун эргүүлэх */
static Elm *rotateRight(Elm *h)
{
        Elm *x = h->L;

        h->L = x->R;
        x->R = h;

        x->color = h->color;
        h->color = RED;

        return x;
}

/* Өнгө солих */
static void flipColors(Elm *h)
{
        h->color = !h->color;

        if (h->L != NULL)
                h->L->color = !h->L->color;

        if (h->R != NULL)
                h->R->color = !h->R->color;
}

/* Тэнцвэр засах */
static Elm *fixUp(Elm *h)
{
        if (isRed(h->R) && !isRed(h->L))
                h = rotateLeft(h);

        if (isRed(h->L) && isRed(h->L->L))
                h = rotateRight(h);

        if (isRed(h->L) && isRed(h->R))
                flipColors(h);

        return h;
}

/* Оруулах туслах функц */
static Elm *insert(Elm *h, const Student *px)
{
        if (h == NULL)
                return newNode(px);

        int cmp = strcmp(px->id, h->x.id);

        if (cmp < 0)
                h->L = insert(h->L, px);
        else if (cmp > 0)
                h->R = insert(h->R, px);
        else
                h->x = *px;

        h = fixUp(h);

        return h;
}

/*
  `ptree`-ийн зааж байгаа модонд `x` утгыг оруулна.
*/
void rb_put(RBT *ptree, const Student *px)
{
        ptree->root = insert(ptree->root, px);

        if (ptree->root != NULL)
                ptree->root->color = BLACK;
}

/*
  `ptree`-ийн зааж байгаа модноос `x` утгыг хайн олдсон оройн `Elm*` хаягийг буцаана.
*/
Elm *rb_get(const RBT *ptree, const char id[])
{
        Elm *cur = ptree->root;

        while (cur != NULL)
        {
                int cmp = strcmp(id, cur->x.id);

                if (cmp < 0)
                        cur = cur->L;
                else if (cmp > 0)
                        cur = cur->R;
                else
                        return cur;
        }

        return NULL;
}

/* h->L->L улаан эсэхийг NULL хамгаалалттай шалгах */
static int isLeftLeftRed(Elm *h)
{
        if (h == NULL || h->L == NULL)
                return 0;

        return isRed(h->L->L);
}

/* h->R->L улаан эсэхийг NULL хамгаалалттай шалгах */
static int isRightLeftRed(Elm *h)
{
        if (h == NULL || h->R == NULL)
                return 0;

        return isRed(h->R->L);
}

static Elm *moveRedLeft(Elm *h)
{
        flipColors(h);

        if (h->R != NULL && isRed(h->R->L))
        {
                h->R = rotateRight(h->R);
                h = rotateLeft(h);
                flipColors(h);
        }

        return h;
}

static Elm *moveRedRight(Elm *h)
{
        flipColors(h);

        if (h->L != NULL && isRed(h->L->L))
        {
                h = rotateRight(h);
                flipColors(h);
        }

        return h;
}

/* Хамгийн бага id-тай оройг олох */
static Elm *minNode(Elm *h)
{
        while (h->L != NULL)
                h = h->L;

        return h;
}

/* Хамгийн бага оройг устгах */
static Elm *deleteMin(Elm *h)
{
        if (h->L == NULL)
        {
                free(h);
                return NULL;
        }

        if (!isRed(h->L) && !isLeftLeftRed(h))
                h = moveRedLeft(h);

        h->L = deleteMin(h->L);

        return fixUp(h);
}

/* Устгах туслах функц */
static Elm *deleteNode(Elm *h, const char id[])
{
        if (strcmp(id, h->x.id) < 0)
        {
                if (h->L != NULL)
                {
                        if (!isRed(h->L) && !isLeftLeftRed(h))
                                h = moveRedLeft(h);

                        h->L = deleteNode(h->L, id);
                }
        }
        else
        {
                if (isRed(h->L))
                        h = rotateRight(h);

                if (strcmp(id, h->x.id) == 0 && h->R == NULL)
                {
                        free(h);
                        return NULL;
                }

                if (h->R != NULL)
                {
                        if (!isRed(h->R) && !isRightLeftRed(h))
                                h = moveRedRight(h);

                        if (strcmp(id, h->x.id) == 0)
                        {
                                Elm *m = minNode(h->R);
                                h->x = m->x;
                                h->R = deleteMin(h->R);
                        }
                        else
                        {
                                h->R = deleteNode(h->R, id);
                        }
                }
        }

        return fixUp(h);
}

/*
  Устгах функц: ТМноос `x` утгыг хайж олоод устгана.
*/
void rb_del(RBT *ptree, const char id[])
{
        if (ptree->root == NULL)
                return;

        if (rb_get(ptree, id) == NULL)
                return;

        if (!isRed(ptree->root->L) && !isRed(ptree->root->R))
                ptree->root->color = RED;

        ptree->root = deleteNode(ptree->root, id);

        if (ptree->root != NULL)
                ptree->root->color = BLACK;
}

void print(Elm *p)
{
        if (p)
                printf("%s %s %d %.1f\n", p->x.name, p->x.id, p->x.age, p->x.gpa);
        else
                printf("None\n");
}
