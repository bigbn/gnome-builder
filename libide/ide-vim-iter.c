/* ide-vim-iter.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

#include "ide-debug.h"
#include "ide-vim-iter.h"

typedef enum
{
  SENTENCE_OK,
  SENTENCE_PARA,
  SENTENCE_FAILED,
} SentenceStatus;

enum
{
  CLASS_0,
  CLASS_SPACE,
  CLASS_SPECIAL,
  CLASS_WORD,
};

static int
_ide_vim_word_classify (gunichar ch)
{
  switch (ch)
    {
    case ' ':
    case '\t':
    case '\n':
      return CLASS_SPACE;

    case '"': case '\'':
    case '(': case ')':
    case '{': case '}':
    case '[': case ']':
    case '<': case '>':
    case '-': case '+': case '*': case '/':
    case '!': case '@': case '#': case '$': case '%':
    case '^': case '&': case ':': case ';': case '?':
    case '|': case '=': case '\\': case '.': case ',':
      return CLASS_SPECIAL;

    case '_':
    default:
      return CLASS_WORD;
    }
}

static int
_ide_vim_WORD_classify (gunichar ch)
{
  if (g_unichar_isspace (ch))
    return CLASS_SPACE;
  return CLASS_WORD;
}

static gboolean
_ide_vim_iter_line_is_empty (GtkTextIter *iter)
{
  return gtk_text_iter_starts_line (iter) && gtk_text_iter_ends_line (iter);
}

/**
 * _ide_vim_iter_backward_paragraph_start:
 * @iter: A #GtkTextIter
 *
 * Searches backwards until we find the beginning of a paragraph.
 *
 * Returns: %TRUE if we are not at the beginning of the buffer; otherwise %FALSE.
 */
gboolean
_ide_vim_iter_backward_paragraph_start (GtkTextIter *iter)
{
  g_return_val_if_fail (iter, FALSE);

  /* Work our way past the current empty lines */
  if (_ide_vim_iter_line_is_empty (iter))
    while (_ide_vim_iter_line_is_empty (iter))
      if (!gtk_text_iter_backward_line (iter))
        return FALSE;

  /* Now find first line that is empty */
  while (!_ide_vim_iter_line_is_empty (iter))
    if (!gtk_text_iter_backward_line (iter))
      return FALSE;

  return TRUE;
}

/**
 * _ide_vim_iter_forward_paragraph_end:
 * @iter: A #GtkTextIter
 *
 * Searches forward until the end of a paragraph has been hit.
 *
 * Returns: %TRUE if we are not at the end of the buffer; otherwise %FALSE.
 */
gboolean
_ide_vim_iter_forward_paragraph_end (GtkTextIter *iter)
{
  g_return_val_if_fail (iter, FALSE);

  /* Work our way past the current empty lines */
  if (_ide_vim_iter_line_is_empty (iter))
    while (_ide_vim_iter_line_is_empty (iter))
      if (!gtk_text_iter_forward_line (iter))
        return FALSE;

  /* Now find first line that is empty */
  while (!_ide_vim_iter_line_is_empty (iter))
    if (!gtk_text_iter_forward_line (iter))
      return FALSE;

  return TRUE;
}

static gboolean
sentence_end_chars (gunichar ch,
                    gpointer user_data)
{
  switch (ch)
    {
    case '!':
    case '.':
    case '?':
      return TRUE;

    default:
      return FALSE;
    }
}

static SentenceStatus
_ide_vim_iter_backward_sentence_end (GtkTextIter *iter)
{
  GtkTextIter end_bounds;
  GtkTextIter start_bounds;
  gboolean found_para;

  g_return_val_if_fail (iter, FALSE);

  end_bounds = *iter;
  start_bounds = *iter;
  found_para = _ide_vim_iter_backward_paragraph_start (&start_bounds);

  if (!found_para)
    gtk_text_buffer_get_start_iter (gtk_text_iter_get_buffer (iter), &start_bounds);

  while ((gtk_text_iter_compare (iter, &start_bounds) > 0) && gtk_text_iter_backward_char (iter))
    {
      if (gtk_text_iter_backward_find_char (iter, sentence_end_chars, NULL, &end_bounds))
        {
          GtkTextIter copy = *iter;

          while (gtk_text_iter_forward_char (&copy) && (gtk_text_iter_compare (&copy, &end_bounds) < 0))
            {
              gunichar ch;

              ch = gtk_text_iter_get_char (&copy);

              switch (ch)
                {
                case ']':
                case ')':
                case '"':
                case '\'':
                  continue;

                case ' ':
                case '\n':
                  *iter = copy;
                  return SENTENCE_OK;

                default:
                  break;
                }
            }
        }
    }

  *iter = start_bounds;

  if (found_para)
    return SENTENCE_PARA;

  return SENTENCE_FAILED;
}

gboolean
_ide_vim_iter_forward_sentence_end (GtkTextIter *iter)
{
  GtkTextIter end_bounds;
  gboolean found_para;

  g_return_val_if_fail (iter, FALSE);

  end_bounds = *iter;
  found_para = _ide_vim_iter_forward_paragraph_end (&end_bounds);

  if (!found_para)
    gtk_text_buffer_get_end_iter (gtk_text_iter_get_buffer (iter), &end_bounds);

  while ((gtk_text_iter_compare (iter, &end_bounds) < 0) && gtk_text_iter_forward_char (iter))
    {
      if (gtk_text_iter_forward_find_char (iter, sentence_end_chars, NULL, &end_bounds))
        {
          GtkTextIter copy = *iter;

          while (gtk_text_iter_forward_char (&copy) && (gtk_text_iter_compare (&copy, &end_bounds) < 0))
            {
              gunichar ch;
              gboolean invalid = FALSE;

              ch = gtk_text_iter_get_char (&copy);

              switch (ch)
                {
                case ']':
                case ')':
                case '"':
                case '\'':
                  continue;

                case ' ':
                case '\n':
                  *iter = copy;
                  return SENTENCE_OK;

                default:
                  invalid = TRUE;
                  break;
                }

              if (invalid)
                break;
            }
        }
    }

  *iter = end_bounds;

  if (found_para)
    return SENTENCE_PARA;

  return SENTENCE_FAILED;
}

gboolean
_ide_vim_iter_backward_sentence_start (GtkTextIter *iter)
{
  GtkTextIter tmp;
  SentenceStatus status;

  g_return_val_if_fail (iter, FALSE);

  tmp = *iter;
  status = _ide_vim_iter_backward_sentence_end (&tmp);

  switch (status)
    {
    case SENTENCE_PARA:
    case SENTENCE_OK:
      {
        GtkTextIter copy = tmp;

        /*
         * try to work forward to first non-whitespace char.
         * if we land where we started, discard the walk.
         */
        while (g_unichar_isspace (gtk_text_iter_get_char (&copy)))
          if (!gtk_text_iter_forward_char (&copy))
            break;
        if (gtk_text_iter_compare (&copy, iter) < 0)
          tmp = copy;
        *iter = tmp;

        return TRUE;
      }

    case SENTENCE_FAILED:
    default:
      gtk_text_buffer_get_start_iter (gtk_text_iter_get_buffer (iter), iter);
      return FALSE;
    }
}

static gboolean
_ide_vim_iter_forward_classified_start (GtkTextIter *iter,
                                        gint (*classify) (gunichar))
{
  gint begin_class;
  gint cur_class;
  gunichar ch;

  g_assert (iter);

  ch = gtk_text_iter_get_char (iter);
  begin_class = classify (ch);

  /* Move to the first non-whitespace character if necessary. */
  if (begin_class == CLASS_SPACE)
    {
      for (;;)
        {
          if (!gtk_text_iter_forward_char (iter))
            return FALSE;

          ch = gtk_text_iter_get_char (iter);
          cur_class = classify (ch);
          if (cur_class != CLASS_SPACE)
            return TRUE;
        }
    }

  /* move to first character not at same class level. */
  while (gtk_text_iter_forward_char (iter))
    {
      ch = gtk_text_iter_get_char (iter);
      cur_class = classify (ch);

      if (cur_class == CLASS_SPACE)
        {
          begin_class = CLASS_0;
          continue;
        }

      if (cur_class != begin_class)
        return TRUE;
    }

  return FALSE;
}

gboolean
_ide_vim_iter_forward_word_start (GtkTextIter *iter)
{
  return _ide_vim_iter_forward_classified_start (iter, _ide_vim_word_classify);
}

gboolean
_ide_vim_iter_forward_WORD_start (GtkTextIter *iter)
{
  return _ide_vim_iter_forward_classified_start (iter, _ide_vim_WORD_classify);
}

gboolean
_ide_vim_iter_forward_classified_end (GtkTextIter *iter,
                                      gint (*classify) (gunichar))
{
  gunichar ch;
  gint begin_class;
  gint cur_class;

  g_assert (iter);

  if (!gtk_text_iter_forward_char (iter))
    return FALSE;

  /* If we are on space, walk to the start of the next word. */
  ch = gtk_text_iter_get_char (iter);
  if (classify (ch) == CLASS_SPACE)
    if (!_ide_vim_iter_forward_classified_start (iter, classify))
      return FALSE;

  ch = gtk_text_iter_get_char (iter);
  begin_class = classify (ch);

  for (;;)
    {
      if (!gtk_text_iter_forward_char (iter))
        return FALSE;

      ch = gtk_text_iter_get_char (iter);
      cur_class = classify (ch);

      if (cur_class != begin_class)
        {
          gtk_text_iter_backward_char (iter);
          return TRUE;
        }
    }

  return FALSE;
}

gboolean
_ide_vim_iter_forward_word_end (GtkTextIter *iter)
{
  return _ide_vim_iter_forward_classified_end (iter, _ide_vim_word_classify);
}

gboolean
_ide_vim_iter_forward_WORD_end (GtkTextIter *iter)
{
  return _ide_vim_iter_forward_classified_end (iter, _ide_vim_WORD_classify);
}

static gboolean
_ide_vim_iter_backward_classified_end (GtkTextIter *iter,
                                       gint (*classify) (gunichar))
{
  gunichar ch;
  gint begin_class;
  gint cur_class;

  g_assert (iter);

  ch = gtk_text_iter_get_char (iter);
  begin_class = classify (ch);

  for (;;)
    {
      if (!gtk_text_iter_backward_char (iter))
        return FALSE;

      ch = gtk_text_iter_get_char (iter);
      cur_class = classify (ch);

      /* reset begin_class if we hit space, we can take anything after that */
      if (cur_class == CLASS_SPACE)
        begin_class = CLASS_SPACE;

      if (cur_class != begin_class && cur_class != CLASS_SPACE)
        return TRUE;
    }

  return FALSE;
}

gboolean
_ide_vim_iter_backward_word_end (GtkTextIter *iter)
{
  return _ide_vim_iter_backward_classified_end (iter, _ide_vim_word_classify);
}

gboolean
_ide_vim_iter_backward_WORD_end (GtkTextIter *iter)
{
  return _ide_vim_iter_backward_classified_end (iter, _ide_vim_WORD_classify);
}
