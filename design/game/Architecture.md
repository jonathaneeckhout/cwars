# Architecture

The architecture of CWars prioritizes **clarity, scalability, and security** over raw performance. This is a deliberate design choice based on the game's unique characteristics:

## Design Philosophy

### Why Not Speed-First?
- **Turn-based nature**: As a programmable RTS where players write bots to issue API commands, the game doesn't require millisecond-level response times
- **Strategic gameplay**: Decision-making happens in player code, not real-time user input
- **Persistent world**: Long-term stability matters more than peak throughput

### Core Priorities

**1. Understandability**
- Clear separation of concerns between game logic, API layer, and state management
- Straightforward request/response patterns that bot developers can reason about
- Well-documented API contracts that make integration predictable

**2. Scalability**
- Horizontal scaling through multiple backend instances behind load balancers (see `design/networking/cwars_network_architecture.drawio`)
- Stateless API servers that can be added/removed as player load changes
- Game state separated from API handlers to enable independent scaling of computation vs. I/O

**3. Security**
- Authentication/authorization baked into every API endpoint
- Rate limiting to prevent bot abuse and ensure fair play
- Input validation to protect against malicious player scripts

### Performance Considerations
While not the primary goal, the architecture still achieves reasonable performance:
- API responses typically under 200ms for standard operations
- Batch endpoints allow efficient multi-action requests
- Game simulation runs asynchronously from API calls

This approach ensures CWars remains maintainable and reliable as the player base grows, while still feeling responsive to bot-driven gameplay.

## Network Architecture

### Component Overview

The CWars network architecture follows a clear separation of concerns with four main layers:

```
[Player Bots/Clients]
        ↓
[Load Balancer]
        ↓
[Multiple Flask API Servers] ← REST endpoints
        ↓                ↓
[User SQL DB]    [Game State SQL DB]
        ↓
   [Redis Streams] ← Event messaging layer
        ↓
[Independent Pipelines]
```

### API Layer

**Flask REST Servers**
- Multiple stateless Flask instances handle incoming API requests from player bots
- Horizontally scalable - new instances can be added behind the load balancer as player load increases
- Each API server is independent and can handle any request type

**Responsibilities:**
- Authenticate requests against the User SQL database
- Validate input parameters and enforce rate limits
- Read current game state from the Game State SQL database
- Publish commands/events to Redis Streams (not direct DB writes)
- Return responses to player clients

### Database Layer

**User SQL Database**
- Stores player accounts, authentication credentials, and session tokens
- Handles login/logout, account management, and access control
- Separate from game state to allow independent scaling and security policies

**Game State SQL Database**
- Stores all game world data: settlements, units, buildings, resources, positions
- Represents the current "snapshot" of the persistent world
- API servers read from this database to answer queries
- Only pipelines write to this database (ensures consistency)

### Event Messaging Layer

**Redis Streams**
- Acts as the event bus between API servers and processing pipelines
- Decouples command submission from execution
- Provides persistence and guaranteed delivery

**Command Streams:**
- API servers publish player commands (e.g., "move_unit", "build_structure", "train_worker")
- Each command includes player authentication, target entities, and parameters
- Commands are immutable once published

**Event Streams:**
- Pipelines publish state change events (e.g., "unit_moved", "building_completed", "combat_resolved")
- Used for notifications, logging, and potential player subscriptions
- Enables event sourcing for replays or debugging

**Consumer Groups:**
- Each pipeline subscribes to relevant command streams via consumer groups
- Ensures each command is processed exactly once
- Allows multiple pipeline instances for horizontal scaling
- Failed processing can be retried or moved to dead letter streams

### Processing Layer

**Independent Pipelines**
- Run separately from the API servers - decoupled architecture
- Each pipeline handles a specific aspect of game simulation
- Consume commands from Redis Streams and apply game rules
- Write results to Game State SQL database
- Publish events back to Redis Streams for notifications

**Pipeline Examples:**
- **Movement Pipeline**: Processes unit movement commands, handles pathfinding, updates positions
- **Production Pipeline**: Executes resource gathering, building production, unit training
- **Combat Pipeline**: Resolves battles, calculates damage, handles unit deaths
- **Survival Pipeline**: Manages hunger mechanics, aging, food consumption
- **Construction Pipeline**: Handles building placement, construction progress, completion

**Key Characteristics:**
- Pipelines run on independent schedules (e.g., every second, every 5 seconds)
- Each pipeline consumes from its dedicated Redis Stream consumer group
- Apply transformations and write results to Game State DB
- Publish completion events back to Redis for tracking
- Failures in one pipeline don't crash others
- Can be scaled independently based on computational needs

### Data Flow Example

**Player issues command: "Move unit to position (100, 50)"**

1. Player bot sends REST request to API server
2. API server authenticates against User DB
3. API server validates unit ownership by querying Game State DB
4. API server publishes command to Redis Stream: `game:commands:movement`
   ```json
   {
     "command": "move_unit",
     "player_id": "abc123",
     "unit_id": "unit_456",
     "target_position": [100, 50],
     "timestamp": "2025-11-22T10:30:00Z"
   }
   ```
5. API server returns success response to player (command accepted, not executed)
6. Movement Pipeline (running independently):
   - Consumes command from Redis Stream via consumer group
   - Reads unit data from Game State DB
   - Calculates path and validates destination
   - Updates unit position in Game State DB
   - Publishes event to Redis Stream: `game:events:movement`
   ```json
   {
     "event": "unit_moved",
     "unit_id": "unit_456",
     "old_position": [50, 25],
     "new_position": [100, 50],
     "timestamp": "2025-11-22T10:30:01Z"
   }
   ```
   - Acknowledges command as processed in Redis
7. Player bot queries unit position in next request and sees updated location
8. (Optional) Player bot subscribes to event stream for real-time notifications

### Benefits of This Architecture

**Responsiveness**: API servers respond quickly without waiting for game logic computation

**Reliability**: Pipeline failures don't affect API availability - commands still get accepted and queued

**Scalability**: Each layer (API, databases, Redis, pipelines) can scale independently based on bottlenecks

**Debuggability**: Each pipeline can be monitored, logged, and debugged separately; Redis Streams provide full audit trail

**Flexibility**: New game mechanics can be added as new pipelines without touching existing code

**Decoupling**: Redis Streams create a clear boundary between command submission and execution

**Event Sourcing**: All commands and events are persisted in Redis Streams, enabling replays, analytics, and debugging

**Exactly-once Processing**: Consumer groups guarantee each command is processed exactly once, preventing duplicate actions

