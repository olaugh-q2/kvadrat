#include "session_state.h"
#include "string_util.h"
#include <stdlib.h>
#include <string.h>

SessionState *CreateSessionState(void) {
  SessionState *session_state = (SessionState *)malloc(sizeof(SessionState));
  session_state->high_score = 0;

  // Initialize dual-lexicon fields to NULL/defaults
  session_state->player_kwg = NULL;
  session_state->opponent_kwg = NULL;
  session_state->sim_mode = SIM_MODE_IGNORANT;
  session_state->player_lexicon_name = NULL;
  session_state->opponent_lexicon_name = NULL;

  return session_state;
}

void DestroySessionState(SessionState *session_state) {
  if (!session_state) {
    return;
  }

  // Don't destroy KWGs here - they may be shared with GameState
  // or managed elsewhere. Just clear our references.

  if (session_state->player_lexicon_name) {
    free(session_state->player_lexicon_name);
  }
  if (session_state->opponent_lexicon_name) {
    free(session_state->opponent_lexicon_name);
  }

  free(session_state);
}

void ConfigureDualLexicon(SessionState *session_state,
                          const char *player_kwg_filename,
                          const char *opponent_kwg_filename, SimMode mode) {
  // Load player's lexicon
  if (session_state->player_kwg) {
    kwg_destroy(session_state->player_kwg);
  }
  session_state->player_kwg = kwg_create(".", player_kwg_filename);

  // Load opponent's lexicon
  if (session_state->opponent_kwg) {
    kwg_destroy(session_state->opponent_kwg);
  }
  session_state->opponent_kwg = kwg_create(".", opponent_kwg_filename);

  session_state->sim_mode = mode;

  // Store names
  if (session_state->player_lexicon_name) {
    free(session_state->player_lexicon_name);
  }
  session_state->player_lexicon_name = string_duplicate(player_kwg_filename);

  if (session_state->opponent_lexicon_name) {
    free(session_state->opponent_lexicon_name);
  }
  session_state->opponent_lexicon_name = string_duplicate(opponent_kwg_filename);
}

const KWG *GetOpponentModelKwg(const SessionState *session_state) {
  if (session_state->sim_mode == SIM_MODE_IGNORANT) {
    // In ignorant mode, we model opponent as using our own lexicon
    return session_state->player_kwg;
  } else {
    // In informed mode, we know opponent's actual lexicon
    return session_state->opponent_kwg;
  }
}

// Helper to check if a word exists in a KWG
static bool word_in_kwg(const KWG *kwg, const char *word) {
  if (!kwg || !word) {
    return false;
  }

  uint32_t node_index = kwg_get_dawg_root_node_index(kwg);

  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    // Convert to uppercase if needed
    if (c >= 'a' && c <= 'z') {
      c = c - 'a' + 'A';
    }
    // Convert letter to tile index (A=1, B=2, etc.)
    uint8_t tile = c - 'A' + 1;

    uint32_t next_index = kwg_get_next_node_index(kwg, node_index, tile);
    if (next_index == 0) {
      return false;
    }
    node_index = next_index;
  }

  // Check if the final node accepts (marks end of valid word)
  // We need to check if we ended at an accepting state
  // Look back at the transition that got us here
  uint32_t check_index = kwg_get_dawg_root_node_index(kwg);
  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    if (c >= 'a' && c <= 'z') {
      c = c - 'a' + 'A';
    }
    uint8_t tile = c - 'A' + 1;

    // Find the node for this tile
    for (uint32_t j = check_index;; j++) {
      uint32_t node = kwg_node(kwg, j);
      if (kwg_node_tile(node) == tile) {
        if (word[i + 1] == '\0') {
          // This is the last letter - check if it accepts
          return kwg_node_accepts(node);
        }
        check_index = kwg_node_arc_index(node);
        break;
      }
      if (kwg_node_is_end(node)) {
        return false;
      }
    }
  }

  return false;
}

bool IsValidForPlayer(const SessionState *session_state, const char *word) {
  return word_in_kwg(session_state->player_kwg, word);
}

bool IsValidForOpponentModel(const SessionState *session_state,
                             const char *word) {
  const KWG *model_kwg = GetOpponentModelKwg(session_state);
  return word_in_kwg(model_kwg, word);
}
