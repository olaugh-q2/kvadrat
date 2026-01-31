// Kvadrat Web - A Tetris word game
// Based on the original Kvadrat by the repository author

// ============================================================================
// Constants
// ============================================================================

const PLAYFIELD_WIDTH = 10;
const PLAYFIELD_HEIGHT = 22;
const VISIBLE_HEIGHT = 20;
const MAX_LINES = 40;
const MIN_WORD_LENGTH = 2;
const MINIMUM_WORD_SCORE = 40;

// Timing (in frames at 60 FPS)
const LATERAL_MOVEMENT_DELAY = 10;
const LATERAL_MOVEMENT_REPEAT_DELAY = 2;
const ROTATION_DELAY = 15;
const SOFT_LOCK_DELAY = 20;
const ENTRY_DELAY = 6;
const LINE_CLEAR_DELAY = 30;
const GRAVITY_DELAY = 48;
const SOFT_DROP_DELAY = 2;

// Piece types
const EMPTY = 0;
const I_PIECE = 1;
const J_PIECE = 2;
const L_PIECE = 3;
const O_PIECE = 4;
const S_PIECE = 5;
const T_PIECE = 6;
const Z_PIECE = 7;

// Rotations
const ROTATION_0 = 0;
const ROTATION_R = 1;
const ROTATION_2 = 2;
const ROTATION_L = 3;

// Piece colors
const PIECE_COLORS = {
    [I_PIECE]: '#00e6fb',
    [J_PIECE]: '#281eff',
    [L_PIECE]: '#ff6622',
    [O_PIECE]: '#ffd835',
    [S_PIECE]: '#30fd39',
    [T_PIECE]: '#ba0ef5',
    [Z_PIECE]: '#ff0638'
};

// Ghost piece colors (dimmed)
const GHOST_COLORS = {
    [I_PIECE]: 'rgba(0, 230, 251, 0.3)',
    [J_PIECE]: 'rgba(40, 30, 255, 0.3)',
    [L_PIECE]: 'rgba(255, 102, 34, 0.3)',
    [O_PIECE]: 'rgba(255, 216, 53, 0.3)',
    [S_PIECE]: 'rgba(48, 253, 57, 0.3)',
    [T_PIECE]: 'rgba(186, 14, 245, 0.3)',
    [Z_PIECE]: 'rgba(255, 6, 56, 0.3)'
};

// Letter point values (Scrabble-based)
const LETTER_VALUES = {
    'A': 1, 'B': 3, 'C': 3, 'D': 2, 'E': 1, 'F': 4, 'G': 2, 'H': 4,
    'I': 1, 'J': 8, 'K': 5, 'L': 1, 'M': 3, 'N': 1, 'O': 1, 'P': 3,
    'Q': 10, 'R': 1, 'S': 1, 'T': 1, 'U': 1, 'V': 4, 'W': 4, 'X': 8,
    'Y': 4, 'Z': 10
};

// Piece shapes for each rotation
const PIECE_SHAPES = {
    [I_PIECE]: {
        [ROTATION_0]: [[0,0], [0,1], [0,2], [0,3]],  // Horizontal at row 1
        [ROTATION_R]: [[0,2], [1,2], [2,2], [3,2]],  // Vertical at col 2
        [ROTATION_2]: [[2,0], [2,1], [2,2], [2,3]],  // Horizontal at row 2
        [ROTATION_L]: [[0,1], [1,1], [2,1], [3,1]]   // Vertical at col 1
    },
    [J_PIECE]: {
        [ROTATION_0]: [[0,0], [1,0], [1,1], [1,2]],
        [ROTATION_R]: [[0,1], [0,2], [1,1], [2,1]],
        [ROTATION_2]: [[1,0], [1,1], [1,2], [2,2]],
        [ROTATION_L]: [[0,1], [1,1], [2,0], [2,1]]
    },
    [L_PIECE]: {
        [ROTATION_0]: [[0,2], [1,0], [1,1], [1,2]],
        [ROTATION_R]: [[0,1], [1,1], [2,1], [2,2]],
        [ROTATION_2]: [[1,0], [1,1], [1,2], [2,0]],
        [ROTATION_L]: [[0,0], [0,1], [1,1], [2,1]]
    },
    [O_PIECE]: {
        [ROTATION_0]: [[0,1], [0,2], [1,1], [1,2]],
        [ROTATION_R]: [[0,1], [0,2], [1,1], [1,2]],
        [ROTATION_2]: [[0,1], [0,2], [1,1], [1,2]],
        [ROTATION_L]: [[0,1], [0,2], [1,1], [1,2]]
    },
    [S_PIECE]: {
        [ROTATION_0]: [[0,1], [0,2], [1,0], [1,1]],
        [ROTATION_R]: [[0,1], [1,1], [1,2], [2,2]],
        [ROTATION_2]: [[1,1], [1,2], [2,0], [2,1]],
        [ROTATION_L]: [[0,0], [1,0], [1,1], [2,1]]
    },
    [T_PIECE]: {
        [ROTATION_0]: [[0,1], [1,0], [1,1], [1,2]],
        [ROTATION_R]: [[0,1], [1,1], [1,2], [2,1]],
        [ROTATION_2]: [[1,0], [1,1], [1,2], [2,1]],
        [ROTATION_L]: [[0,1], [1,0], [1,1], [2,1]]
    },
    [Z_PIECE]: {
        [ROTATION_0]: [[0,0], [0,1], [1,1], [1,2]],
        [ROTATION_R]: [[0,2], [1,1], [1,2], [2,1]],
        [ROTATION_2]: [[1,0], [1,1], [2,1], [2,2]],
        [ROTATION_L]: [[0,1], [1,0], [1,1], [2,0]]
    }
};

// SRS Wall kick data
const WALL_KICKS = {
    'JLSTZ': {
        '0>R': [[0, 0], [0, -1], [-1, -1], [2, 0], [2, -1]],
        'R>0': [[0, 0], [0, 1], [1, 1], [-2, 0], [-2, 1]],
        'R>2': [[0, 0], [0, 1], [1, 1], [-2, 0], [-2, 1]],
        '2>R': [[0, 0], [0, -1], [-1, -1], [2, 0], [2, -1]],
        '2>L': [[0, 0], [0, 1], [-1, 1], [2, 0], [2, 1]],
        'L>2': [[0, 0], [0, -1], [1, -1], [-2, 0], [-2, -1]],
        'L>0': [[0, 0], [0, -1], [1, -1], [-2, 0], [-2, -1]],
        '0>L': [[0, 0], [0, 1], [-1, 1], [2, 0], [2, 1]]
    },
    'I': {
        '0>R': [[0, 0], [0, -2], [0, 1], [-1, -2], [2, 1]],
        'R>0': [[0, 0], [0, 2], [0, -1], [1, 2], [-2, -1]],
        'R>2': [[0, 0], [0, -1], [0, 2], [2, -1], [-1, 2]],
        '2>R': [[0, 0], [0, 1], [0, -2], [-2, 1], [1, -2]],
        '2>L': [[0, 0], [0, 2], [0, -1], [1, 2], [-2, -1]],
        'L>2': [[0, 0], [0, -2], [0, 1], [-1, -2], [2, 1]],
        'L>0': [[0, 0], [0, 1], [0, -2], [-2, 1], [1, -2]],
        '0>L': [[0, 0], [0, -1], [0, 2], [2, -1], [-1, 2]]
    }
};

const ROTATION_NAMES = ['0', 'R', '2', 'L'];

// ============================================================================
// Game State
// ============================================================================

let canvas, ctx, nextCanvas, nextCtx;
let cellSize = 24;
let gameState = null;

class GameState {
    constructor() {
        this.reset();
    }

    reset() {
        // Playfield (22 rows, 10 cols) - each cell is {piece, letter}
        this.grid = [];
        for (let r = 0; r < PLAYFIELD_HEIGHT; r++) {
            this.grid.push([]);
            for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
                this.grid[r].push({ piece: EMPTY, letter: null });
            }
        }

        // Piece queue and bag
        this.pieceQueue = [];
        this.letterBag = [];
        this.fillPieceQueue();
        this.fillLetterBag();

        // Current piece state
        this.currentPiece = null;
        this.currentRotation = ROTATION_0;
        this.currentRow = 0;
        this.currentCol = 3;
        this.currentLetters = [];
        this.ghostRow = 0;

        // Timers
        this.gravityCounter = 0;
        this.lateralCounter = 0;
        this.lateralDirection = 0;
        this.lateralRepeating = false;
        this.rotationCounter = 0;
        this.rotationDirection = 0;
        this.softLockCounter = 0;
        this.softLocking = false;
        this.lockCounter = 0;
        this.locking = false;
        this.lineClearCounter = 0;
        this.clearingLines = false;
        this.clearedRows = [];

        // Stats
        this.score = 0;
        this.lines = 0;
        this.wordCount = 0;
        this.wordsFormed = [];

        // Game state
        this.paused = false;
        this.gameOver = false;
        this.gameWon = false;
        this.softDropping = false;
        this.hardDropped = false;

        // Input state
        this.keys = {};

        // Spawn first piece
        this.spawnPiece();
    }

    fillPieceQueue() {
        // Generate bags of 7 pieces (one of each)
        while (this.pieceQueue.length < 14) {
            const bag = [I_PIECE, J_PIECE, L_PIECE, O_PIECE, S_PIECE, T_PIECE, Z_PIECE];
            shuffleArray(bag);
            this.pieceQueue.push(...bag);
        }
    }

    fillLetterBag() {
        // Generate letter bags based on word-friendly distribution
        const letterCounts = {
            'E': 12, 'T': 9, 'A': 9, 'O': 8, 'I': 7, 'N': 7, 'S': 6, 'H': 6,
            'R': 6, 'D': 4, 'L': 4, 'C': 3, 'U': 3, 'M': 3, 'W': 2, 'F': 2,
            'G': 2, 'Y': 2, 'P': 2, 'B': 2, 'V': 2, 'K': 1, 'J': 1, 'X': 1,
            'Q': 1, 'Z': 1
        };

        const letters = [];
        for (const [letter, count] of Object.entries(letterCounts)) {
            for (let i = 0; i < count; i++) {
                letters.push(letter);
            }
        }
        shuffleArray(letters);
        this.letterBag.push(...letters);
    }

    getNextLetters(count) {
        while (this.letterBag.length < count) {
            this.fillLetterBag();
        }
        return this.letterBag.splice(0, count);
    }

    spawnPiece() {
        this.fillPieceQueue();
        this.currentPiece = this.pieceQueue.shift();
        this.currentRotation = ROTATION_0;
        this.currentRow = 0;
        this.currentCol = 3;
        this.currentLetters = this.getNextLetters(4);
        this.softLocking = false;
        this.softLockCounter = 0;
        this.locking = false;
        this.lockCounter = 0;
        this.hardDropped = false;

        // Check for game over (collision at spawn)
        if (this.checkCollision(this.currentPiece, this.currentRotation, this.currentRow, this.currentCol)) {
            this.gameOver = true;
            this.showGameOver();
        }

        this.updateGhost();
    }

    checkCollision(piece, rotation, row, col) {
        const shape = PIECE_SHAPES[piece][rotation];
        for (const [dr, dc] of shape) {
            const r = row + dr;
            const c = col + dc;
            if (r < 0 || r >= PLAYFIELD_HEIGHT || c < 0 || c >= PLAYFIELD_WIDTH) {
                return true;
            }
            if (this.grid[r][c].piece !== EMPTY) {
                return true;
            }
        }
        return false;
    }

    updateGhost() {
        let ghostRow = this.currentRow;
        while (!this.checkCollision(this.currentPiece, this.currentRotation, ghostRow + 1, this.currentCol)) {
            ghostRow++;
        }
        this.ghostRow = ghostRow;
    }

    movePiece(direction) {
        const newCol = this.currentCol + direction;
        if (!this.checkCollision(this.currentPiece, this.currentRotation, this.currentRow, newCol)) {
            this.currentCol = newCol;
            this.updateGhost();
            if (this.softLocking) {
                this.softLockCounter = 0;
            }
            return true;
        }
        return false;
    }

    rotatePiece(direction) {
        const newRotation = (this.currentRotation + direction + 4) % 4;
        const kickTable = this.currentPiece === I_PIECE ? 'I' : 'JLSTZ';
        const kickKey = `${ROTATION_NAMES[this.currentRotation]}>${ROTATION_NAMES[newRotation]}`;
        const kicks = WALL_KICKS[kickTable][kickKey];

        if (kicks) {
            for (const [dr, dc] of kicks) {
                const newRow = this.currentRow - dr; // Note: row is inverted
                const newCol = this.currentCol + dc;
                if (!this.checkCollision(this.currentPiece, newRotation, newRow, newCol)) {
                    this.currentRotation = newRotation;
                    this.currentRow = newRow;
                    this.currentCol = newCol;
                    this.updateGhost();
                    if (this.softLocking) {
                        this.softLockCounter = 0;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    dropPiece() {
        if (!this.checkCollision(this.currentPiece, this.currentRotation, this.currentRow + 1, this.currentCol)) {
            this.currentRow++;
            this.softLocking = false;
            this.softLockCounter = 0;
            return true;
        } else {
            this.softLocking = true;
            return false;
        }
    }

    hardDrop() {
        while (this.dropPiece()) {}
        this.hardDropped = true;
        this.lockPiece();
    }

    lockPiece() {
        const shape = PIECE_SHAPES[this.currentPiece][this.currentRotation];
        for (let i = 0; i < shape.length; i++) {
            const [dr, dc] = shape[i];
            const r = this.currentRow + dr;
            const c = this.currentCol + dc;
            if (r >= 0 && r < PLAYFIELD_HEIGHT) {
                this.grid[r][c] = {
                    piece: this.currentPiece,
                    letter: this.currentLetters[i]
                };
            }
        }

        this.checkLineClears();
    }

    checkLineClears() {
        this.clearedRows = [];
        for (let r = 0; r < PLAYFIELD_HEIGHT; r++) {
            let full = true;
            for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
                if (this.grid[r][c].piece === EMPTY) {
                    full = false;
                    break;
                }
            }
            if (full) {
                this.clearedRows.push(r);
            }
        }

        if (this.clearedRows.length > 0) {
            this.clearingLines = true;
            this.lineClearCounter = LINE_CLEAR_DELAY;
            this.findWords();
        } else {
            this.spawnPiece();
        }
    }

    findWords() {
        // Find horizontal words in cleared lines
        for (const row of this.clearedRows) {
            let word = '';
            let startCol = 0;

            for (let c = 0; c <= PLAYFIELD_WIDTH; c++) {
                const cell = c < PLAYFIELD_WIDTH ? this.grid[row][c] : { letter: null };

                if (cell.letter) {
                    if (word === '') startCol = c;
                    word += cell.letter;
                } else {
                    if (word.length >= MIN_WORD_LENGTH) {
                        this.checkWord(word, row, startCol);
                    }
                    word = '';
                }
            }
        }
    }

    checkWord(word, row, col) {
        const lowerWord = word.toLowerCase();
        if (typeof VALID_WORDS !== 'undefined' && VALID_WORDS.has(lowerWord)) {
            const score = this.calculateWordScore(word);
            if (score >= MINIMUM_WORD_SCORE) {
                this.score += score;
                this.wordCount++;
                this.wordsFormed.push({ word: word.toUpperCase(), score, row, col });
                updateStats();
            }
        }
    }

    calculateWordScore(word) {
        let baseScore = 0;
        for (const letter of word.toUpperCase()) {
            baseScore += LETTER_VALUES[letter] || 0;
        }
        return baseScore * word.length * word.length;
    }

    clearLines() {
        // Remove cleared rows and add empty ones at top
        for (const row of this.clearedRows.sort((a, b) => b - a)) {
            this.grid.splice(row, 1);
            const newRow = [];
            for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
                newRow.push({ piece: EMPTY, letter: null });
            }
            this.grid.unshift(newRow);
        }

        this.lines += this.clearedRows.length;

        // Base points for line clears (even without words)
        const linePoints = [0, 100, 300, 500, 800];
        this.score += linePoints[this.clearedRows.length] || 0;

        updateStats();

        if (this.lines >= MAX_LINES) {
            this.gameWon = true;
            this.showGameOver();
        }

        this.clearedRows = [];
        this.clearingLines = false;
        this.spawnPiece();
    }

    update() {
        if (this.paused || this.gameOver || this.gameWon) return;

        // Handle line clearing animation
        if (this.clearingLines) {
            this.lineClearCounter--;
            if (this.lineClearCounter <= 0) {
                this.clearLines();
            }
            return;
        }

        // Handle piece locking animation
        if (this.locking) {
            this.lockCounter--;
            if (this.lockCounter <= 0) {
                this.lockPiece();
                this.locking = false;
            }
            return;
        }

        // Handle lateral movement
        if (this.lateralDirection !== 0) {
            if (this.lateralCounter <= 0) {
                this.movePiece(this.lateralDirection);
                this.lateralCounter = this.lateralRepeating ? LATERAL_MOVEMENT_REPEAT_DELAY : LATERAL_MOVEMENT_DELAY;
                this.lateralRepeating = true;
            }
            this.lateralCounter--;
        }

        // Handle rotation
        if (this.rotationDirection !== 0) {
            if (this.rotationCounter <= 0) {
                this.rotatePiece(this.rotationDirection);
                this.rotationCounter = ROTATION_DELAY;
            }
            this.rotationCounter--;
        }

        // Handle gravity
        const gravityDelay = this.softDropping ? SOFT_DROP_DELAY : GRAVITY_DELAY;
        this.gravityCounter++;
        if (this.gravityCounter >= gravityDelay) {
            this.gravityCounter = 0;
            if (!this.dropPiece()) {
                // Piece landed
                if (this.softLocking) {
                    this.softLockCounter++;
                    if (this.softLockCounter >= SOFT_LOCK_DELAY) {
                        this.locking = true;
                        this.lockCounter = ENTRY_DELAY;
                    }
                }
            }
        }
    }

    showGameOver() {
        const overlay = document.getElementById('pause-overlay');
        const title = document.getElementById('overlay-title');
        const button = document.getElementById('overlay-button');

        if (this.gameWon) {
            title.textContent = 'YOU WIN!';
            document.body.classList.add('game-won');
            document.body.classList.remove('game-over');
        } else {
            title.textContent = 'GAME OVER';
            document.body.classList.add('game-over');
            document.body.classList.remove('game-won');
        }

        document.getElementById('overlay-score').textContent = `Score: ${this.score}`;
        document.getElementById('overlay-lines').textContent = `Lines: ${this.lines}`;
        document.getElementById('overlay-words').textContent = `Words: ${this.wordCount}`;
        button.textContent = 'Play Again';
        button.onclick = () => {
            overlay.style.display = 'none';
            document.body.classList.remove('game-over', 'game-won');
            saveHighScore();
            this.reset();
            updateStats();
        };
        overlay.style.display = 'flex';
    }

    togglePause() {
        if (this.gameOver || this.gameWon) return;

        this.paused = !this.paused;
        const overlay = document.getElementById('pause-overlay');

        if (this.paused) {
            document.getElementById('overlay-title').textContent = 'PAUSED';
            document.getElementById('overlay-score').textContent = `Score: ${this.score}`;
            document.getElementById('overlay-lines').textContent = `Lines: ${this.lines}`;
            document.getElementById('overlay-words').textContent = `Words: ${this.wordCount}`;
            const button = document.getElementById('overlay-button');
            button.textContent = 'Resume';
            button.onclick = () => this.togglePause();
            overlay.style.display = 'flex';
        } else {
            overlay.style.display = 'none';
        }
    }
}

// ============================================================================
// Rendering
// ============================================================================

function render() {
    if (!gameState) return;

    // Clear canvas
    ctx.fillStyle = '#16213e';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw grid lines
    ctx.strokeStyle = '#2a2a4a';
    ctx.lineWidth = 1;
    for (let r = 0; r <= VISIBLE_HEIGHT; r++) {
        ctx.beginPath();
        ctx.moveTo(0, r * cellSize);
        ctx.lineTo(PLAYFIELD_WIDTH * cellSize, r * cellSize);
        ctx.stroke();
    }
    for (let c = 0; c <= PLAYFIELD_WIDTH; c++) {
        ctx.beginPath();
        ctx.moveTo(c * cellSize, 0);
        ctx.lineTo(c * cellSize, VISIBLE_HEIGHT * cellSize);
        ctx.stroke();
    }

    // Draw placed pieces
    for (let r = 2; r < PLAYFIELD_HEIGHT; r++) {
        for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
            const cell = gameState.grid[r][c];
            if (cell.piece !== EMPTY) {
                const isClearing = gameState.clearingLines && gameState.clearedRows.includes(r);
                if (isClearing) {
                    // Flash effect for clearing lines
                    const flash = Math.floor(gameState.lineClearCounter / 5) % 2 === 0;
                    if (flash) {
                        drawCell(c, r - 2, cell.piece, cell.letter, 0.5);
                    } else {
                        drawCell(c, r - 2, cell.piece, cell.letter, 1);
                    }
                } else {
                    drawCell(c, r - 2, cell.piece, cell.letter, 1);
                }
            }
        }
    }

    // Draw ghost piece
    if (gameState.currentPiece && !gameState.clearingLines && !gameState.locking) {
        const shape = PIECE_SHAPES[gameState.currentPiece][gameState.currentRotation];
        for (let i = 0; i < shape.length; i++) {
            const [dr, dc] = shape[i];
            const r = gameState.ghostRow + dr - 2;
            const c = gameState.currentCol + dc;
            if (r >= 0) {
                drawGhostCell(c, r, gameState.currentPiece, gameState.currentLetters[i]);
            }
        }
    }

    // Draw current piece
    if (gameState.currentPiece && !gameState.clearingLines && !gameState.locking) {
        const shape = PIECE_SHAPES[gameState.currentPiece][gameState.currentRotation];
        for (let i = 0; i < shape.length; i++) {
            const [dr, dc] = shape[i];
            const r = gameState.currentRow + dr - 2;
            const c = gameState.currentCol + dc;
            if (r >= 0) {
                drawCell(c, r, gameState.currentPiece, gameState.currentLetters[i], 1);
            }
        }
    }

    // Draw recent words
    renderWords();

    // Draw next pieces
    renderNext();
}

function drawCell(col, row, piece, letter, alpha) {
    const x = col * cellSize;
    const y = row * cellSize;
    const color = PIECE_COLORS[piece];

    // Fill
    ctx.globalAlpha = alpha;
    ctx.fillStyle = color;
    ctx.fillRect(x + 1, y + 1, cellSize - 2, cellSize - 2);

    // Highlight
    ctx.fillStyle = 'rgba(255, 255, 255, 0.3)';
    ctx.fillRect(x + 1, y + 1, cellSize - 2, 4);
    ctx.fillRect(x + 1, y + 1, 4, cellSize - 2);

    // Shadow
    ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
    ctx.fillRect(x + cellSize - 5, y + 1, 4, cellSize - 2);
    ctx.fillRect(x + 1, y + cellSize - 5, cellSize - 2, 4);

    // Letter
    if (letter) {
        ctx.fillStyle = '#000';
        ctx.font = `bold ${cellSize * 0.6}px Arial`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(letter, x + cellSize / 2, y + cellSize / 2 + 1);
    }

    ctx.globalAlpha = 1;
}

function drawGhostCell(col, row, piece, letter) {
    const x = col * cellSize;
    const y = row * cellSize;

    ctx.strokeStyle = PIECE_COLORS[piece];
    ctx.lineWidth = 2;
    ctx.strokeRect(x + 2, y + 2, cellSize - 4, cellSize - 4);

    if (letter) {
        ctx.fillStyle = 'rgba(255, 255, 255, 0.3)';
        ctx.font = `bold ${cellSize * 0.5}px Arial`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(letter, x + cellSize / 2, y + cellSize / 2 + 1);
    }
}

function renderNext() {
    const previewSize = cellSize * 0.8;
    nextCtx.fillStyle = '#16213e';
    nextCtx.fillRect(0, 0, nextCanvas.width, nextCanvas.height);

    for (let i = 0; i < 5 && i < gameState.pieceQueue.length; i++) {
        const piece = gameState.pieceQueue[i];
        const shape = PIECE_SHAPES[piece][ROTATION_0];
        const offsetY = i * (previewSize * 3);

        for (const [dr, dc] of shape) {
            const x = dc * previewSize + previewSize * 0.5;
            const y = dr * previewSize + offsetY + previewSize * 0.5;

            nextCtx.fillStyle = PIECE_COLORS[piece];
            nextCtx.fillRect(x + 1, y + 1, previewSize - 2, previewSize - 2);

            // Highlight
            nextCtx.fillStyle = 'rgba(255, 255, 255, 0.3)';
            nextCtx.fillRect(x + 1, y + 1, previewSize - 2, 3);
        }
    }
}

function renderWords() {
    // Display recently formed words on screen
    const now = Date.now();
    gameState.wordsFormed = gameState.wordsFormed.filter(w => {
        if (!w.time) w.time = now;
        return now - w.time < 3000;
    });

    for (const wordInfo of gameState.wordsFormed) {
        const elapsed = now - wordInfo.time;
        const alpha = Math.max(0, 1 - elapsed / 3000);
        const y = (wordInfo.row - 2) * cellSize - 20 - elapsed * 0.02;
        const x = wordInfo.col * cellSize + 10;

        ctx.globalAlpha = alpha;
        ctx.fillStyle = '#4ade80';
        ctx.font = 'bold 14px Arial';
        ctx.textAlign = 'left';
        ctx.fillText(`${wordInfo.word} +${wordInfo.score}`, x, y);
        ctx.globalAlpha = 1;
    }
}

// ============================================================================
// Input Handling
// ============================================================================

function setupInput() {
    // Keyboard input
    document.addEventListener('keydown', (e) => {
        if (gameState.gameOver || gameState.gameWon) return;

        switch (e.code) {
            case 'ArrowLeft':
            case 'KeyA':
                if (gameState.lateralDirection !== -1) {
                    gameState.lateralDirection = -1;
                    gameState.lateralCounter = 0;
                    gameState.lateralRepeating = false;
                }
                e.preventDefault();
                break;
            case 'ArrowRight':
            case 'KeyD':
                if (gameState.lateralDirection !== 1) {
                    gameState.lateralDirection = 1;
                    gameState.lateralCounter = 0;
                    gameState.lateralRepeating = false;
                }
                e.preventDefault();
                break;
            case 'ArrowDown':
            case 'KeyS':
                gameState.softDropping = true;
                e.preventDefault();
                break;
            case 'ArrowUp':
            case 'KeyW':
            case 'Space':
                if (!gameState.paused) {
                    gameState.hardDrop();
                }
                e.preventDefault();
                break;
            case 'KeyZ':
                if (gameState.rotationDirection !== -1) {
                    gameState.rotationDirection = -1;
                    gameState.rotationCounter = 0;
                }
                e.preventDefault();
                break;
            case 'KeyX':
                if (gameState.rotationDirection !== 1) {
                    gameState.rotationDirection = 1;
                    gameState.rotationCounter = 0;
                }
                e.preventDefault();
                break;
            case 'KeyP':
            case 'Escape':
                gameState.togglePause();
                e.preventDefault();
                break;
        }
    });

    document.addEventListener('keyup', (e) => {
        switch (e.code) {
            case 'ArrowLeft':
            case 'KeyA':
                if (gameState.lateralDirection === -1) {
                    gameState.lateralDirection = 0;
                    gameState.lateralRepeating = false;
                }
                break;
            case 'ArrowRight':
            case 'KeyD':
                if (gameState.lateralDirection === 1) {
                    gameState.lateralDirection = 0;
                    gameState.lateralRepeating = false;
                }
                break;
            case 'ArrowDown':
            case 'KeyS':
                gameState.softDropping = false;
                break;
            case 'KeyZ':
                if (gameState.rotationDirection === -1) {
                    gameState.rotationDirection = 0;
                }
                break;
            case 'KeyX':
                if (gameState.rotationDirection === 1) {
                    gameState.rotationDirection = 0;
                }
                break;
        }
    });

    // Touch controls
    setupTouchButton('btn-left', () => {
        gameState.lateralDirection = -1;
        gameState.lateralCounter = 0;
        gameState.lateralRepeating = false;
    }, () => {
        gameState.lateralDirection = 0;
    });

    setupTouchButton('btn-right', () => {
        gameState.lateralDirection = 1;
        gameState.lateralCounter = 0;
        gameState.lateralRepeating = false;
    }, () => {
        gameState.lateralDirection = 0;
    });

    setupTouchButton('btn-down', () => {
        gameState.softDropping = true;
    }, () => {
        gameState.softDropping = false;
    });

    setupTouchButton('btn-hard', () => {
        if (!gameState.paused) gameState.hardDrop();
    });

    setupTouchButton('btn-ccw', () => {
        gameState.rotatePiece(-1);
    });

    setupTouchButton('btn-cw', () => {
        gameState.rotatePiece(1);
    });
}

function setupTouchButton(id, onDown, onUp) {
    const btn = document.getElementById(id);
    if (!btn) return;

    const handleDown = (e) => {
        e.preventDefault();
        if (gameState.gameOver || gameState.gameWon) return;
        onDown();
    };

    const handleUp = (e) => {
        e.preventDefault();
        if (onUp) onUp();
    };

    btn.addEventListener('touchstart', handleDown);
    btn.addEventListener('mousedown', handleDown);
    if (onUp) {
        btn.addEventListener('touchend', handleUp);
        btn.addEventListener('mouseup', handleUp);
        btn.addEventListener('mouseleave', handleUp);
    }
}

// ============================================================================
// Stats and Storage
// ============================================================================

function updateStats() {
    document.getElementById('score-display').textContent = gameState.score;
    document.getElementById('lines-display').textContent = `${gameState.lines}/${MAX_LINES}`;
    document.getElementById('words-display').textContent = gameState.wordCount;

    const highScore = getHighScore();
    document.getElementById('high-score-display').textContent = Math.max(highScore, gameState.score);
}

function getHighScore() {
    return parseInt(localStorage.getItem('kvadrat-high-score') || '0');
}

function saveHighScore() {
    const current = getHighScore();
    if (gameState.score > current) {
        localStorage.setItem('kvadrat-high-score', gameState.score.toString());
    }
}

// ============================================================================
// Utilities
// ============================================================================

function shuffleArray(array) {
    for (let i = array.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [array[i], array[j]] = [array[j], array[i]];
    }
}

function resizeCanvas() {
    const container = document.getElementById('game-container');
    const maxWidth = container.clientWidth - 180;
    const maxHeight = container.clientHeight - 150;

    const widthBasedSize = Math.floor(maxWidth / PLAYFIELD_WIDTH);
    const heightBasedSize = Math.floor(maxHeight / VISIBLE_HEIGHT);

    cellSize = Math.min(widthBasedSize, heightBasedSize, 30);
    cellSize = Math.max(cellSize, 16);

    canvas.width = PLAYFIELD_WIDTH * cellSize;
    canvas.height = VISIBLE_HEIGHT * cellSize;

    const previewSize = cellSize * 0.8;
    nextCanvas.width = previewSize * 4;
    nextCanvas.height = previewSize * 15;
}

// ============================================================================
// Main
// ============================================================================

function gameLoop() {
    gameState.update();
    render();
    requestAnimationFrame(gameLoop);
}

function init() {
    canvas = document.getElementById('main-canvas');
    ctx = canvas.getContext('2d');
    nextCanvas = document.getElementById('next-canvas');
    nextCtx = nextCanvas.getContext('2d');

    resizeCanvas();
    window.addEventListener('resize', resizeCanvas);

    gameState = new GameState();
    setupInput();
    updateStats();

    // Start at ~60 FPS
    setInterval(() => gameState.update(), 1000 / 60);
    gameLoop();
}

// Wait for DOM and word list
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}
