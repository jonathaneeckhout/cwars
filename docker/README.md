# CWars Docker Infrastructure

This document explains how to run the CWars backend infrastructure using Docker Compose.

**Note:** All docker-related files are in the `docker/` directory. Run commands from this directory.

## Overview

The docker-compose setup includes:
- **Nginx**: Load balancer with rate limiting (ready for API servers)
- **PostgreSQL (User DB)**: Player accounts and authentication
- **PostgreSQL (Game State DB)**: World state and game data
- **Redis**: Event bus for commands and events
- **Redis Commander**: Web UI for Redis (dev only)
- **PgAdmin**: PostgreSQL web UI (dev only)

## Quick Start

### 0. Navigate to docker directory
```bash
cd docker
```

### 1. Copy environment file
```bash
cp .env.example .env
```

Edit `.env` and set secure passwords for production.

### 2. Start infrastructure (production mode)
```bash
docker-compose up -d
```

This starts only the core services (no dev tools).

### 3. Start with dev tools
```bash
docker-compose --profile dev up -d
```

This includes Redis Commander (port 8081) and PgAdmin (port 5050).

### 4. Check service health
```bash
docker-compose ps
```

All services should show "healthy" status.

## Service Access

### Production Services
- **Nginx Load Balancer**: http://localhost:80
- **User Database**: localhost:5432 (internal only)
- **Game State Database**: localhost:5432 (internal only)
- **Redis**: localhost:6379 (internal only)

### Development Tools (--profile dev)
- **Redis Commander**: http://localhost:8081
- **PgAdmin**: http://localhost:5050
  - Login: admin@cwars.local / changeme_admin (or your .env values)

## Database Initialization

The databases are created as empty PostgreSQL instances. Schema will be defined and created using SQLAlchemy models when the Flask API servers are implemented.

## Connecting to Services

### PostgreSQL (User DB)
```bash
docker-compose exec user-db psql -U cwars_user -d cwars_users
```

### PostgreSQL (Game State DB)
```bash
docker-compose exec game-state-db psql -U cwars_game -d cwars_gamestate
```

### Redis CLI
```bash
docker-compose exec redis redis-cli
```

## Nginx Configuration

The Nginx load balancer is pre-configured with:
- Rate limiting per endpoint type (auth, query, command, batch)
- WebSocket support for real-time events
- Health check endpoint at `/health`

**Rate Limits (per minute per IP):**
- Authentication: 10 requests
- Query (GET): 100 requests
- Commands (POST): 30 requests
- Batch: 10 requests

### Adding API Servers

Edit `nginx/nginx.conf` and update the `upstream api_servers` block:

```nginx
upstream api_servers {
    server api-server-1:5000;
    server api-server-2:5000;
    server api-server-3:5000;
}
```

Then reload Nginx:
```bash
docker-compose exec nginx nginx -s reload
```

## Data Persistence

All data is persisted in Docker volumes:
- `user-db-data` - Player accounts
- `game-state-db-data` - Game world state
- `redis-data` - Redis streams and cache
- `pgadmin-data` - PgAdmin settings (dev only)

### Backup Data
```bash
# Backup user database
docker-compose exec user-db pg_dump -U cwars_user cwars_users > backup_users.sql

# Backup game state database
docker-compose exec game-state-db pg_dump -U cwars_game cwars_gamestate > backup_gamestate.sql

# Backup Redis
docker-compose exec redis redis-cli SAVE
docker cp cwars-redis:/data/dump.rdb ./backup_redis.rdb
```

### Restore Data
```bash
# Restore user database
cat backup_users.sql | docker-compose exec -T user-db psql -U cwars_user cwars_users

# Restore game state database
cat backup_gamestate.sql | docker-compose exec -T game-state-db psql -U cwars_game cwars_gamestate
```

## Stopping Services

### Stop all services
```bash
docker-compose down
```

### Stop and remove volumes (WARNING: deletes all data)
```bash
docker-compose down -v
```

## Monitoring

### View logs
```bash
# All services
docker-compose logs -f

# Specific service
docker-compose logs -f nginx
docker-compose logs -f redis
docker-compose logs -f user-db
docker-compose logs -f game-state-db
```

### Check resource usage
```bash
docker stats
```

## Troubleshooting

### Nginx returns 503
- This is expected until API servers are added to the upstream configuration
- The health check endpoint (`/health`) should still return 200 OK

### Database connection refused
- Check if containers are healthy: `docker-compose ps`
- Verify passwords in `.env` file
- Check logs: `docker-compose logs user-db` or `docker-compose logs game-state-db`

### Redis connection issues
- Verify Redis is running: `docker-compose exec redis redis-cli ping`
- Should return `PONG`

### Port conflicts
If ports 80, 5432, 6379, 8081, or 5050 are already in use:
- Edit `docker-compose.yml` and change port mappings
- Example: `"8080:80"` instead of `"80:80"`

## Network Architecture

All services communicate on the `cwars-network` bridge network (subnet 172.20.0.0/16).

**Service Communication:**
- API servers will connect to: `user-db:5432`, `game-state-db:5432`, `redis:6379`
- Nginx forwards requests to: `api_servers` upstream
- External access only through Nginx on port 80/443

## Next Steps

1. Implement Flask API servers (connect to databases and Redis)
2. Implement processing pipelines (consume Redis Streams)
3. Add API server containers to docker-compose
4. Configure Nginx upstream with API server addresses
5. Implement WebSocket server for real-time events

## Security Notes

**For Production:**
- Change all default passwords in `.env`
- Use strong, randomly generated passwords
- Enable SSL/TLS in Nginx (certificates in `nginx/ssl/`)
- Restrict database access to internal network only
- Use Docker secrets instead of environment variables
- Enable firewall rules to limit external access
- Regular security updates: `docker-compose pull && docker-compose up -d`
