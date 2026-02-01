#ifndef SESSION_STATE_H
#define SESSION_STATE_H

#include "kwg.h"
#include <stdbool.h>

// Simulation modes for dual-lexicon play
typedef enum {
  // Each player assumes opponent uses the same lexicon they do
  // (TWL player thinks opponent also uses TWL, doesn't see CSW-only hooks)
  SIM_MODE_IGNORANT = 0,

  // Each player knows both lexicons - they know what words the opponent
  // can play, but each player is constrained to use only their own lexicon
  SIM_MODE_INFORMED = 1
} SimMode;

typedef struct {
  int high_score;

  // Dual-lexicon simulation support
  KWG *player_kwg;      // Lexicon for the player being simulated
  KWG *opponent_kwg;    // Lexicon for the opponent
  SimMode sim_mode;     // Which simulation mode to use

  // Names for display/debugging
  char *player_lexicon_name;
  char *opponent_lexicon_name;
} SessionState;

SessionState *CreateSessionState(void);
void DestroySessionState(SessionState *session_state);

// Dual-lexicon configuration
void ConfigureDualLexicon(SessionState *session_state,
                          const char *player_kwg_filename,
                          const char *opponent_kwg_filename,
                          SimMode mode);

// Get the KWG to use when evaluating opponent's possible moves
// In IGNORANT mode: returns player's own KWG (assumes opponent uses same)
// In INFORMED mode: returns opponent's actual KWG
const KWG *GetOpponentModelKwg(const SessionState *session_state);

// Check if a word is valid for the player
bool IsValidForPlayer(const SessionState *session_state, const char *word);

// Check if a word is valid for the opponent (according to current sim mode)
bool IsValidForOpponentModel(const SessionState *session_state, const char *word);

#endif // SESSION_STATE_H
