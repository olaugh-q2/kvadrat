// Kvadrat Web - A Tetris word game
// Based on the original Kvadrat - uses same KWG dictionary and scoring logic

// ============================================================================
// Constants
// ============================================================================

const PLAYFIELD_WIDTH = 10;
const PLAYFIELD_HEIGHT = 22;
const VISIBLE_HEIGHT = 20;
const MAX_LINES = 40;
const MINIMUM_WORD_SCORE = 40;

// Timing (in frames at 60 FPS)
const LATERAL_MOVEMENT_DELAY = 10;
const LATERAL_MOVEMENT_REPEAT_DELAY = 2;
const ROTATION_DELAY = 30;
const SOFT_LOCK_DELAY = 20;
const ENTRY_DELAY = 6;
const LINE_CLEAR_DELAY = 30;
const GRAVITY_DELAY = 12;
const SOFT_DROP_DELAY = 1;

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

// Piece colors (matching original)
const PIECE_COLORS = {
    [I_PIECE]: '#00e6fb',
    [J_PIECE]: '#281eff',
    [L_PIECE]: '#ff6622',
    [O_PIECE]: '#ffd835',
    [S_PIECE]: '#30fd39',
    [T_PIECE]: '#ba0ef5',
    [Z_PIECE]: '#ff0638'
};

// Letter point values (matching bag.h)
const LETTER_SCORES = [
    0,  // unused (index 0)
    1,  // A
    3,  // B
    3,  // C
    2,  // D
    1,  // E
    4,  // F
    2,  // G
    4,  // H
    1,  // I
    8,  // J
    5,  // K
    1,  // L
    3,  // M
    1,  // N
    1,  // O
    3,  // P
    10, // Q
    1,  // R
    1,  // S
    1,  // T
    1,  // U
    4,  // V
    4,  // W
    8,  // X
    4,  // Y
    10  // Z
];

// Piece shapes for each rotation (row, col offsets for each of 4 blocks)
// These match tetrominos.c CreatePiece function
const PIECE_SHAPES = {
    [I_PIECE]: {
        [ROTATION_0]: [[1,0], [1,1], [1,2], [1,3]],
        [ROTATION_R]: [[0,2], [1,2], [2,2], [3,2]],
        [ROTATION_2]: [[2,0], [2,1], [2,2], [2,3]],
        [ROTATION_L]: [[0,1], [1,1], [2,1], [3,1]]
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
        [ROTATION_0]: [[1,1], [1,2], [2,1], [2,2]],
        [ROTATION_R]: [[1,1], [1,2], [2,1], [2,2]],
        [ROTATION_2]: [[1,1], [1,2], [2,1], [2,2]],
        [ROTATION_L]: [[1,1], [1,2], [2,1], [2,2]]
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
// KWG (Kleene Word Graph) - Dictionary for word validation
// ============================================================================

class KWG {
    constructor(buffer) {
        // KWG file is array of uint32_t in little-endian
        const dataView = new DataView(buffer);
        const nodeCount = buffer.byteLength / 4;
        this.nodes = new Uint32Array(nodeCount);

        for (let i = 0; i < nodeCount; i++) {
            this.nodes[i] = dataView.getUint32(i * 4, true); // little-endian
        }

        // Get DAWG root from node 0
        this.dawgRoot = this.nodes[0] & 0x3FFFFF;
    }

    getNode(index) {
        return this.nodes[index];
    }

    nodeTile(node) {
        return node >>> 24;
    }

    nodeAccepts(node) {
        return (node & 0x800000) !== 0;
    }

    nodeIsEnd(node) {
        return (node & 0x400000) !== 0;
    }

    nodeArcIndex(node) {
        return node & 0x3FFFFF;
    }

    // Get next node index for a given letter (1-26 for A-Z)
    getNextNodeIndex(nodeIndex, letter) {
        let i = nodeIndex;
        while (true) {
            const node = this.nodes[i];
            if (this.nodeTile(node) === letter) {
                return this.nodeArcIndex(node);
            }
            if (this.nodeIsEnd(node)) {
                return 0;
            }
            i++;
        }
    }

    // Check if a word is valid (letters as array of 1-26)
    isValidWord(letters) {
        if (letters.length < 2) return false;

        let nodeIndex = this.dawgRoot;
        for (let i = 0; i < letters.length; i++) {
            const letter = letters[i];
            let found = false;
            let accepts = false;

            for (let j = nodeIndex; ; j++) {
                const node = this.nodes[j];
                if (this.nodeTile(node) === letter) {
                    nodeIndex = this.nodeArcIndex(node);
                    accepts = this.nodeAccepts(node);
                    found = true;

                    // If this is the last letter, check if word is complete
                    if (i === letters.length - 1) {
                        return accepts;
                    }
                    break;
                }
                if (this.nodeIsEnd(node)) {
                    return false;
                }
            }

            if (!found || nodeIndex === 0) {
                return false;
            }
        }
        return false;
    }
}

// ============================================================================
// Global State
// ============================================================================

let canvas, ctx, nextCanvas, nextCtx;
let cellSize = 24;
let gameState = null;
let kwg = null;
let letterBags = [];

// ============================================================================
// Asset Loading
// ============================================================================

async function loadAssets() {
    const loadingBar = document.getElementById('loading-bar-fill');
    const loadingText = document.getElementById('loading-text');

    try {
        // Load KWG dictionary
        loadingText.textContent = 'Loading dictionary...';
        loadingBar.style.width = '20%';

        const kwgResponse = await fetch('CSW21.kwg');
        if (!kwgResponse.ok) throw new Error('Failed to load dictionary');
        const kwgBuffer = await kwgResponse.arrayBuffer();
        kwg = new KWG(kwgBuffer);

        loadingBar.style.width = '60%';

        // Load letter bags
        loadingText.textContent = 'Loading word bags...';
        const bagsResponse = await fetch('csw21-bags.txt');
        if (!bagsResponse.ok) throw new Error('Failed to load word bags');
        const bagsText = await bagsResponse.text();

        // Parse bags - each line has 28 four-letter words separated by spaces
        const lines = bagsText.trim().split('\n');
        for (const line of lines) {
            const words = line.trim().split(' ');
            const bag = [];
            for (const word of words) {
                if (word.length === 4) {
                    bag.push(word.split('').map(c => c.charCodeAt(0) - 64)); // A=1, B=2, etc.
                }
            }
            if (bag.length === 28) {
                letterBags.push(bag);
            }
        }

        loadingBar.style.width = '100%';
        loadingText.textContent = 'Ready!';

        await new Promise(r => setTimeout(r, 300));
        document.getElementById('loading-screen').style.display = 'none';

        return true;
    } catch (error) {
        loadingText.textContent = 'Error: ' + error.message;
        console.error('Failed to load assets:', error);
        return false;
    }
}

// ============================================================================
// Game State
// ============================================================================

class GameState {
    constructor() {
        this.reset();
    }

    reset() {
        // Playfield grid - each cell is {piece, letter} where letter is 1-26 (A-Z)
        this.grid = [];
        for (let r = 0; r < PLAYFIELD_HEIGHT; r++) {
            this.grid.push([]);
            for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
                this.grid[r].push({ piece: EMPTY, letter: 0 });
            }
        }

        // Piece queue (7-bag randomizer)
        this.pieceQueue = [];
        this.piecesUntilRedraw = 0;
        this.fillPieceQueue();

        // Letter bags
        this.currentBag = null;
        this.currentBagIndex = 0;
        this.wordsUntilRedraw = 0;
        this.wordLetters = []; // Array of [4 letters] for each piece
        this.drawWordsFromBag();

        // Current piece state
        this.currentPiece = null;
        this.currentRotation = ROTATION_0;
        this.currentRow = 0;
        this.currentCol = 3;
        this.currentLetters = [0, 0, 0, 0];
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

        // Word tracking per row
        this.horizontalWordIds = [];
        this.horizontalWordScores = [];
        for (let r = 0; r < PLAYFIELD_HEIGHT; r++) {
            this.horizontalWordIds.push(new Array(PLAYFIELD_WIDTH).fill(0));
            this.horizontalWordScores.push(new Array(PLAYFIELD_WIDTH).fill(0));
        }

        // Stats
        this.score = 0;
        this.lines = 0;
        this.wordCount = 0;
        this.wordsFormed = [];
        this.frameCounter = 0;

        // Game state
        this.paused = false;
        this.gameOver = false;
        this.gameWon = false;
        this.softDropping = false;
        this.hardDropped = false;

        // Spawn first piece
        this.spawnPiece();
    }

    fillPieceQueue() {
        while (this.pieceQueue.length < 14) {
            const bag = [I_PIECE, J_PIECE, L_PIECE, O_PIECE, S_PIECE, T_PIECE, Z_PIECE];
            shuffleArray(bag);
            this.pieceQueue.push(...bag);
        }
    }

    drawWordsFromBag() {
        if (letterBags.length === 0) {
            // Fallback if bags not loaded
            for (let i = 0; i < 56; i++) {
                this.wordLetters.push([
                    Math.floor(Math.random() * 26) + 1,
                    Math.floor(Math.random() * 26) + 1,
                    Math.floor(Math.random() * 26) + 1,
                    Math.floor(Math.random() * 26) + 1
                ]);
            }
            this.wordsUntilRedraw = 28;
            return;
        }

        // Pick a random bag
        const bagIndex = Math.floor(Math.random() * letterBags.length);
        const bag = letterBags[bagIndex];

        // Add 28 letter groups to wordLetters
        for (let i = 0; i < 28; i++) {
            this.wordLetters.push([...bag[i]]);
        }
        this.wordsUntilRedraw = 28;
    }

    getNextLetters() {
        // Shift letters and refill if needed
        this.wordsUntilRedraw--;
        const letters = this.wordLetters.shift();

        if (this.wordsUntilRedraw === 0) {
            this.drawWordsFromBag();
        }

        return letters || [1, 1, 1, 1]; // Fallback to A's
    }

    spawnPiece() {
        this.fillPieceQueue();

        // Shift piece queue
        this.currentPiece = this.pieceQueue.shift();
        this.currentRotation = ROTATION_0;
        this.currentRow = 1;
        this.currentCol = 3;
        this.currentLetters = this.getNextLetters();
        this.softLocking = false;
        this.softLockCounter = 0;
        this.locking = false;
        this.lockCounter = 0;
        this.hardDropped = false;

        // Try spawn at row 1, then row 0
        if (this.checkCollision(this.currentPiece, this.currentRotation, this.currentRow, this.currentCol)) {
            this.currentRow = 0;
            if (this.checkCollision(this.currentPiece, this.currentRotation, this.currentRow, this.currentCol)) {
                this.gameOver = true;
                this.showGameOver();
                return;
            }
        }

        this.updateGhost();
    }

    checkCollision(piece, rotation, row, col) {
        const shape = PIECE_SHAPES[piece][rotation];
        for (let i = 0; i < 4; i++) {
            const [dr, dc] = shape[i];
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
                const newRow = this.currentRow + dr;
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
        this.placePiece();
        this.markFormedWords();
        this.checkLineClears();
    }

    placePiece() {
        const shape = PIECE_SHAPES[this.currentPiece][this.currentRotation];
        for (let i = 0; i < 4; i++) {
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
    }

    // Mark formed words using the same algorithm as the C version
    markFormedWords() {
        if (!kwg) return;

        for (let row = 0; row < PLAYFIELD_HEIGHT; row++) {
            // Reset word markings for this row
            for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                this.horizontalWordIds[row][col] = 0;
                this.horizontalWordScores[row][col] = 0;
            }

            // Find best word combination for this row
            const bestMarking = new Array(PLAYFIELD_WIDTH).fill(0);
            const bestScores = new Array(PLAYFIELD_WIDTH).fill(0);
            const marking = new Array(PLAYFIELD_WIDTH).fill(0);
            const scores = new Array(PLAYFIELD_WIDTH).fill(0);

            this.findBestHorizontalWords(
                kwg.dawgRoot, kwg.dawgRoot, false,
                row, 0, -1, 1, false, 0,
                bestMarking, bestScores, marking, scores
            );

            for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                this.horizontalWordIds[row][col] = bestMarking[col];
                this.horizontalWordScores[row][col] = bestScores[col];
            }
        }
    }

    // Recursive word finding - matches MarkBestHorizontalWords from game_state.c
    findBestHorizontalWords(dawgRoot, nodeIndex, accepts, row, currentCol, wordStartCol,
                            nextWordId, colorChanged, previousColor,
                            bestMarking, bestScores, marking, scores) {
        let endedWord = false;

        // If we changed colors and previous path accepts, we found a word
        if (colorChanged && accepts) {
            const score = this.scoreHorizontalWord(row, wordStartCol, currentCol - 1);
            if (score >= MINIMUM_WORD_SCORE) {
                scores[wordStartCol] = score;
                for (let col = wordStartCol + 1; col <= currentCol - 1; col++) {
                    scores[col] = 0;
                }
                for (let col = wordStartCol; col <= currentCol - 1; col++) {
                    marking[col] = nextWordId;
                }
                endedWord = true;
                nextWordId++;
            }
        }

        // End of row - compare scores
        if (currentCol >= PLAYFIELD_WIDTH) {
            let bestSum = 0, thisSum = 0;
            for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                bestSum += bestScores[col];
                thisSum += scores[col];
            }
            if (thisSum > bestSum) {
                for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                    bestMarking[col] = marking[col];
                    bestScores[col] = scores[col];
                }
            }
            return;
        }

        const cell = this.grid[row][currentCol];

        // Empty square - reset and continue
        if (cell.piece === EMPTY) {
            const newMarking = [...marking];
            const newScores = [...scores];
            this.findBestHorizontalWords(dawgRoot, dawgRoot, false, row, currentCol + 1,
                -1, nextWordId, false, 0, bestMarking, bestScores, newMarking, newScores);
            return;
        }

        const color = cell.piece;
        if (endedWord) {
            colorChanged = false;
        } else {
            colorChanged = colorChanged || (color && previousColor && color !== previousColor);
        }

        if (endedWord) {
            const newMarking = [...marking];
            const newScores = [...scores];
            this.findBestHorizontalWords(dawgRoot, dawgRoot, false, row, currentCol,
                -1, nextWordId, false, 0, bestMarking, bestScores, newMarking, newScores);
            return;
        }

        const letter = cell.letter;
        let nextNode = 0;
        let newAccepts = false;

        // Search KWG for this letter
        for (let i = nodeIndex; ; i++) {
            const node = kwg.getNode(i);
            if (kwg.nodeTile(node) === letter) {
                nextNode = kwg.nodeArcIndex(node);
                newAccepts = kwg.nodeAccepts(node);
                break;
            }
            if (kwg.nodeIsEnd(node)) {
                break;
            }
        }

        if (wordStartCol < 0) {
            wordStartCol = currentCol;
        }

        // Try ending word at this letter (if color changed and accepts)
        if (colorChanged && newAccepts) {
            const newMarking2 = [...marking];
            const newScores2 = [...scores];
            this.findBestHorizontalWords(dawgRoot, nextNode, newAccepts, row, currentCol + 1,
                wordStartCol, nextWordId, colorChanged, color,
                bestMarking, bestScores, newMarking2, newScores2);
        }

        // Continue word
        const newMarking3 = [...marking];
        const newScores3 = [...scores];
        this.findBestHorizontalWords(dawgRoot, nextNode, false, row, currentCol + 1,
            wordStartCol, nextWordId, colorChanged, color,
            bestMarking, bestScores, newMarking3, newScores3);

        // Start new word at next position
        const newMarking4 = [...marking];
        const newScores4 = [...scores];
        this.findBestHorizontalWords(dawgRoot, dawgRoot, false, row, currentCol + 1,
            -1, nextWordId, false, 0,
            bestMarking, bestScores, newMarking4, newScores4);
    }

    scoreHorizontalWord(row, startCol, endCol) {
        let sum = 0;
        const length = endCol - startCol + 1;
        for (let col = startCol; col <= endCol; col++) {
            const letter = this.grid[row][col].letter;
            if (letter >= 1 && letter <= 26) {
                sum += LETTER_SCORES[letter];
            }
        }
        return sum * length * length;
    }

    checkLineClears() {
        this.clearedRows = [];
        let scoreSum = 0;

        for (let row = 0; row < PLAYFIELD_HEIGHT; row++) {
            let full = true;
            for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                if (this.grid[row][col].piece === EMPTY) {
                    full = false;
                    break;
                }
            }
            if (full) {
                this.clearedRows.push(row);

                // Collect words and scores from this row
                for (let col = 0; col < PLAYFIELD_WIDTH; col++) {
                    const wordScore = this.horizontalWordScores[row][col];
                    if (wordScore > 0) {
                        // Find word length
                        const wordId = this.horizontalWordIds[row][col];
                        let wordLength = 1;
                        let word = String.fromCharCode(this.grid[row][col].letter + 64);
                        for (let col2 = col + 1; col2 < PLAYFIELD_WIDTH; col2++) {
                            if (this.horizontalWordIds[row][col2] === wordId) {
                                wordLength++;
                                word += String.fromCharCode(this.grid[row][col2].letter + 64);
                            } else {
                                break;
                            }
                        }

                        scoreSum += wordScore;
                        this.wordCount++;
                        this.wordsFormed.push({
                            word: word,
                            score: wordScore,
                            row: row,
                            col: col,
                            time: Date.now()
                        });
                    }
                }
            }
        }

        if (this.clearedRows.length > 0) {
            this.clearingLines = true;
            this.lineClearCounter = LINE_CLEAR_DELAY;
            this.score += scoreSum;
            updateStats();
        } else {
            this.spawnPiece();
        }
    }

    clearLines() {
        // Remove cleared rows from bottom to top
        for (const row of this.clearedRows.sort((a, b) => b - a)) {
            this.grid.splice(row, 1);
            const newRow = [];
            for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
                newRow.push({ piece: EMPTY, letter: 0 });
            }
            this.grid.unshift(newRow);

            // Also shift word tracking arrays
            this.horizontalWordIds.splice(row, 1);
            this.horizontalWordIds.unshift(new Array(PLAYFIELD_WIDTH).fill(0));
            this.horizontalWordScores.splice(row, 1);
            this.horizontalWordScores.unshift(new Array(PLAYFIELD_WIDTH).fill(0));
        }

        this.lines += this.clearedRows.length;
        updateStats();

        if (this.lines >= MAX_LINES) {
            this.gameWon = true;
            this.showGameOver();
            return;
        }

        this.clearedRows = [];
        this.clearingLines = false;
        this.spawnPiece();
    }

    update() {
        if (this.paused || this.gameOver || this.gameWon) return;

        this.frameCounter++;

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
                this.locking = false;
            }
            return;
        }

        // Handle lateral movement
        if (this.lateralDirection !== 0) {
            this.lateralCounter++;
            const delay = this.lateralRepeating ? LATERAL_MOVEMENT_REPEAT_DELAY : LATERAL_MOVEMENT_DELAY;
            if (this.lateralCounter >= delay) {
                this.movePiece(this.lateralDirection);
                this.lateralCounter = 0;
                this.lateralRepeating = true;
            }
        }

        // Handle rotation
        if (this.rotationDirection !== 0) {
            this.rotationCounter++;
            if (this.rotationCounter >= ROTATION_DELAY) {
                this.rotationCounter = 0;
                this.rotationDirection = 0;
            }
        }

        // Handle gravity
        const gravityDelay = this.softDropping ? SOFT_DROP_DELAY : GRAVITY_DELAY;
        this.gravityCounter++;
        if (this.gravityCounter >= gravityDelay) {
            this.gravityCounter = 0;
            if (!this.dropPiece()) {
                if (this.softLocking) {
                    this.softLockCounter++;
                    if (this.softLockCounter >= SOFT_LOCK_DELAY) {
                        this.locking = true;
                        this.lockCounter = ENTRY_DELAY;
                        this.lockPiece();
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

    // Draw placed pieces (only visible rows: 2-21 map to screen 0-19)
    for (let r = 2; r < PLAYFIELD_HEIGHT; r++) {
        for (let c = 0; c < PLAYFIELD_WIDTH; c++) {
            const cell = gameState.grid[r][c];
            if (cell.piece !== EMPTY) {
                const isClearing = gameState.clearingLines && gameState.clearedRows.includes(r);
                if (isClearing) {
                    const flash = Math.floor(gameState.lineClearCounter / 5) % 2 === 0;
                    drawCell(c, r - 2, cell.piece, cell.letter, flash ? 0.5 : 1);
                } else {
                    drawCell(c, r - 2, cell.piece, cell.letter, 1);
                }
            }
        }
    }

    // Draw ghost piece
    if (gameState.currentPiece && !gameState.clearingLines && !gameState.locking) {
        const shape = PIECE_SHAPES[gameState.currentPiece][gameState.currentRotation];
        for (let i = 0; i < 4; i++) {
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
        for (let i = 0; i < 4; i++) {
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
    if (letter >= 1 && letter <= 26) {
        ctx.fillStyle = '#000';
        ctx.font = `bold ${cellSize * 0.55}px Arial`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(String.fromCharCode(letter + 64), x + cellSize / 2, y + cellSize / 2 + 1);
    }

    ctx.globalAlpha = 1;
}

function drawGhostCell(col, row, piece, letter) {
    const x = col * cellSize;
    const y = row * cellSize;

    ctx.strokeStyle = PIECE_COLORS[piece];
    ctx.lineWidth = 2;
    ctx.strokeRect(x + 2, y + 2, cellSize - 4, cellSize - 4);

    if (letter >= 1 && letter <= 26) {
        ctx.fillStyle = 'rgba(255, 255, 255, 0.3)';
        ctx.font = `bold ${cellSize * 0.45}px Arial`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(String.fromCharCode(letter + 64), x + cellSize / 2, y + cellSize / 2 + 1);
    }
}

function renderNext() {
    const previewSize = cellSize * 0.75;
    nextCtx.fillStyle = '#16213e';
    nextCtx.fillRect(0, 0, nextCanvas.width, nextCanvas.height);

    for (let i = 0; i < 5 && i < gameState.pieceQueue.length; i++) {
        const piece = gameState.pieceQueue[i];
        const shape = PIECE_SHAPES[piece][ROTATION_0];
        const offsetY = i * (previewSize * 2.5);
        const letters = gameState.wordLetters[i] || [0, 0, 0, 0];

        for (let j = 0; j < 4; j++) {
            const [dr, dc] = shape[j];
            const x = dc * previewSize + previewSize * 0.3;
            const y = dr * previewSize + offsetY;

            nextCtx.fillStyle = PIECE_COLORS[piece];
            nextCtx.fillRect(x + 1, y + 1, previewSize - 2, previewSize - 2);

            // Highlight
            nextCtx.fillStyle = 'rgba(255, 255, 255, 0.3)';
            nextCtx.fillRect(x + 1, y + 1, previewSize - 2, 2);

            // Letter
            if (letters[j] >= 1 && letters[j] <= 26) {
                nextCtx.fillStyle = '#000';
                nextCtx.font = `bold ${previewSize * 0.5}px Arial`;
                nextCtx.textAlign = 'center';
                nextCtx.textBaseline = 'middle';
                nextCtx.fillText(String.fromCharCode(letters[j] + 64), x + previewSize / 2, y + previewSize / 2 + 1);
            }
        }
    }
}

function renderWords() {
    const now = Date.now();
    gameState.wordsFormed = gameState.wordsFormed.filter(w => now - w.time < 3000);

    for (const wordInfo of gameState.wordsFormed) {
        const elapsed = now - wordInfo.time;
        const alpha = Math.max(0, 1 - elapsed / 3000);
        const y = (wordInfo.row - 2) * cellSize - 10 - elapsed * 0.015;
        const x = wordInfo.col * cellSize;

        if (y < 0) continue;

        ctx.globalAlpha = alpha;
        ctx.fillStyle = '#4ade80';
        ctx.font = 'bold 12px Arial';
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
        if (!gameState || gameState.gameOver || gameState.gameWon) return;

        switch (e.code) {
            case 'ArrowLeft':
            case 'KeyA':
                if (gameState.lateralDirection !== -1) {
                    gameState.lateralDirection = -1;
                    gameState.lateralCounter = LATERAL_MOVEMENT_DELAY;
                    gameState.lateralRepeating = false;
                }
                e.preventDefault();
                break;
            case 'ArrowRight':
            case 'KeyD':
                if (gameState.lateralDirection !== 1) {
                    gameState.lateralDirection = 1;
                    gameState.lateralCounter = LATERAL_MOVEMENT_DELAY;
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
                if (!gameState.paused && !gameState.hardDropped) {
                    gameState.hardDrop();
                }
                e.preventDefault();
                break;
            case 'KeyZ':
                if (gameState.rotationCounter === 0) {
                    gameState.rotatePiece(-1);
                    gameState.rotationCounter = 1;
                }
                e.preventDefault();
                break;
            case 'KeyX':
            case 'KeyC':
                if (gameState.rotationCounter === 0) {
                    gameState.rotatePiece(1);
                    gameState.rotationCounter = 1;
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
        if (!gameState) return;

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
            case 'KeyX':
            case 'KeyC':
                gameState.rotationCounter = 0;
                break;
        }
    });

    // Touch input
    setupTouchInput();
}

function setupTouchInput() {
    const wrapper = document.getElementById('play-area-wrapper');
    let touchStartX = 0;
    let touchStartY = 0;
    let touchStartTime = 0;
    let hasMoved = false;

    wrapper.addEventListener('touchstart', (e) => {
        if (!gameState || gameState.gameOver || gameState.gameWon || gameState.paused) return;

        const touch = e.touches[0];
        touchStartX = touch.clientX;
        touchStartY = touch.clientY;
        touchStartTime = Date.now();
        hasMoved = false;
        e.preventDefault();
    }, { passive: false });

    wrapper.addEventListener('touchmove', (e) => {
        if (!gameState || gameState.gameOver || gameState.gameWon || gameState.paused) return;

        const touch = e.touches[0];
        const deltaX = touch.clientX - touchStartX;
        const deltaY = touch.clientY - touchStartY;
        const threshold = cellSize * 0.8;

        // Horizontal swipe - move piece
        if (Math.abs(deltaX) > threshold) {
            hasMoved = true;
            gameState.movePiece(deltaX > 0 ? 1 : -1);
            touchStartX = touch.clientX;
        }

        // Vertical swipe down - soft drop
        if (deltaY > threshold) {
            hasMoved = true;
            gameState.softDropping = true;
            touchStartY = touch.clientY;
        }

        e.preventDefault();
    }, { passive: false });

    wrapper.addEventListener('touchend', (e) => {
        if (!gameState || gameState.gameOver || gameState.gameWon) return;

        gameState.softDropping = false;

        const touchEndTime = Date.now();
        const touchDuration = touchEndTime - touchStartTime;

        // If it was a tap (short duration, no significant movement)
        if (!hasMoved && touchDuration < 300) {
            const touch = e.changedTouches[0];
            const rect = canvas.getBoundingClientRect();
            const x = touch.clientX - rect.left;
            const canvasWidth = rect.width;

            // Check for swipe up (hard drop)
            const deltaY = touch.clientY - touchStartY;
            if (deltaY < -cellSize * 1.5) {
                gameState.hardDrop();
                return;
            }

            // Tap on left 25% - rotate CCW
            if (x < canvasWidth * 0.25) {
                gameState.rotatePiece(-1);
            }
            // Tap on right 25% - rotate CW
            else if (x > canvasWidth * 0.75) {
                gameState.rotatePiece(1);
            }
            // Tap in middle - hard drop
            else {
                gameState.hardDrop();
            }
        } else {
            // Check for swipe up (hard drop)
            const touch = e.changedTouches[0];
            const deltaY = touch.clientY - touchStartY;
            if (deltaY < -cellSize * 2) {
                gameState.hardDrop();
            }
        }

        e.preventDefault();
    }, { passive: false });
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
    const isMobile = window.innerWidth <= 700;
    const maxWidth = isMobile ? container.clientWidth - 20 : container.clientWidth - 200;
    const maxHeight = isMobile ? container.clientHeight - 200 : container.clientHeight - 50;

    const widthBasedSize = Math.floor(maxWidth / PLAYFIELD_WIDTH);
    const heightBasedSize = Math.floor(maxHeight / VISIBLE_HEIGHT);

    cellSize = Math.min(widthBasedSize, heightBasedSize, 32);
    cellSize = Math.max(cellSize, 18);

    canvas.width = PLAYFIELD_WIDTH * cellSize;
    canvas.height = VISIBLE_HEIGHT * cellSize;

    const previewSize = cellSize * 0.75;
    nextCanvas.width = previewSize * 4.5;
    nextCanvas.height = previewSize * 12.5;
}

// ============================================================================
// Main
// ============================================================================

let lastFrameTime = 0;
const frameInterval = 1000 / 60;

function gameLoop(timestamp) {
    if (timestamp - lastFrameTime >= frameInterval) {
        if (gameState) {
            gameState.update();
        }
        lastFrameTime = timestamp;
    }
    render();
    requestAnimationFrame(gameLoop);
}

async function init() {
    canvas = document.getElementById('main-canvas');
    ctx = canvas.getContext('2d');
    nextCanvas = document.getElementById('next-canvas');
    nextCtx = nextCanvas.getContext('2d');

    // Load assets first
    const loaded = await loadAssets();
    if (!loaded) {
        return;
    }

    resizeCanvas();
    window.addEventListener('resize', resizeCanvas);

    gameState = new GameState();
    setupInput();
    updateStats();

    requestAnimationFrame(gameLoop);
}

// Start
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}
