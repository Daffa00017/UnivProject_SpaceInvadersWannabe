using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using RestAPI.Data;
using RestAPI.Models;
using System.Text.Json;

namespace RestAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")] // /api/scores
    public class ScoresController : ControllerBase
    {
        private readonly LeaderboardDbContext _db;
        public ScoresController(LeaderboardDbContext db) => _db = db;

        // ---------------- Minimal endpoint (optional; creates a new row per POST) ----------------
        public record SubmitScoreDto(string PlayerName, int Score);

        // POST /api/scores
        [HttpPost]
        public async Task<IActionResult> Submit([FromBody] SubmitScoreDto dto)
        {
            if (string.IsNullOrWhiteSpace(dto.PlayerName)) return BadRequest("PlayerName required");
            if (dto.Score < 0) return BadRequest("Score must be >= 0");

            var entry = new HighScore
            {
                PlayerName = dto.PlayerName.Trim(),
                Score = dto.Score,
                AchievedAt = DateTime.UtcNow  // use DateTime, not DateTimeOffset, to avoid SQLite ordering issues
            };
            _db.HighScores.Add(entry);
            await _db.SaveChangesAsync();
            return CreatedAtAction(nameof(GetPlayerBest), new { name = entry.PlayerName }, entry);
        }

        // ---------------- Queries ----------------

        // GET /api/scores/top?limit=10
        [HttpGet("top")]
        public async Task<IActionResult> GetTop([FromQuery] int limit = 10)
        {
            limit = Math.Clamp(limit, 1, 100);

            var top = await _db.HighScores
                .AsNoTracking()
                .OrderByDescending(s => s.Score)
                .ThenBy(s => s.Id) // tie-break without DateTimeOffset
                .Take(limit)
                .ToListAsync();

            return Ok(top);
        }

        // GET /api/scores/player/{name}/best
        [HttpGet("player/{name}/best")]
        public async Task<IActionResult> GetPlayerBest(string name)
        {
            if (string.IsNullOrWhiteSpace(name)) return BadRequest("name required");

            var best = await _db.HighScores
                .AsNoTracking()
                .Where(s => s.PlayerName == name)
                .OrderByDescending(s => s.Score)
                .ThenBy(s => s.Id) // tie-break
                .FirstOrDefaultAsync();

            return best is null ? NotFound() : Ok(best);
        }

        // (Optional) GET /api/scores/player/{name}/history
        [HttpGet("player/{name}/history")]
        public async Task<IActionResult> GetPlayerHistory(string name)
        {
            var scores = await _db.HighScores
                .AsNoTracking()
                .Where(s => s.PlayerName == name)
                .OrderByDescending(s => s.Id) // newest first without DateTimeOffset ORDER BY
                .ToListAsync();
            return Ok(scores);
        }

        // GET /api/scores/all
        [HttpGet("all")]
        public async Task<IActionResult> GetAll()
        {
            var rows = await _db.HighScores
                .AsNoTracking()
                .OrderByDescending(s => s.Score)
                .ThenBy(s => s.Id)
                .ToListAsync();
            return Ok(rows);
        }

        // ---------------- Extended "single row per player" upsert ----------------

        public sealed class ScoreSubmitDto
        {
            public string Username { get; set; } = "";
            public string? Password { get; set; }     // plain for dev only
            public int Score { get; set; }
            public int Level { get; set; }
            public double TimeSec { get; set; }
            public string SelectedShipId { get; set; } = "Classic";
            public List<string> UnlockedShips { get; set; } = new();
            public double LastUpdatedSec { get; set; }
        }

        // POST /api/scores/submit
        [HttpPost("submit")]
        public async Task<IActionResult> SubmitExtended([FromBody] ScoreSubmitDto e)
        {
            if (string.IsNullOrWhiteSpace(e.Username)) return BadRequest("username required");

            var row = await _db.HighScores.FirstOrDefaultAsync(x => x.PlayerName == e.Username);

            // simple password check: if a password exists, require match
            if (row != null && !string.IsNullOrEmpty(row.Password) &&
                !string.IsNullOrEmpty(e.Password) &&
                !string.Equals(row.Password, e.Password, StringComparison.Ordinal))
            {
                return Unauthorized(new { error = "incorrect password" });
            }

            // Merge unlocks (store as JSON)
            List<string> merged;
            if (row?.UnlockedShipsJson is string have && !string.IsNullOrWhiteSpace(have))
            {
                var cur = JsonSerializer.Deserialize<List<string>>(have) ?? new();
                merged = cur.Concat(e.UnlockedShips).Distinct(StringComparer.Ordinal).ToList();
            }
            else
            {
                merged = e.UnlockedShips.Distinct(StringComparer.Ordinal).ToList();
            }
            var unlocksJson = JsonSerializer.Serialize(merged);

            if (row == null)
            {
                row = new HighScore
                {
                    PlayerName = e.Username,
                    Password = string.IsNullOrEmpty(e.Password) ? null : e.Password, // set once
                    Score = e.Score,
                    Level = e.Level,
                    TimeSec = e.TimeSec,
                    AchievedAt = DateTime.UtcNow, // DateTime not DateTimeOffset
                    SelectedShipId = e.SelectedShipId,
                    UnlockedShipsJson = unlocksJson,
                    LastUpdatedSec = e.LastUpdatedSec
                };
                _db.HighScores.Add(row);
            }
            else
            {
                if (string.IsNullOrEmpty(row.Password) && !string.IsNullOrEmpty(e.Password))
                    row.Password = e.Password;

                // Best score wins; tie-breaker lower TimeSec is better
                if (e.Score > row.Score || (e.Score == row.Score && e.TimeSec < row.TimeSec))
                {
                    row.Score = e.Score;
                    row.Level = e.Level;
                    row.TimeSec = e.TimeSec;
                    row.AchievedAt = DateTime.UtcNow;
                }
                else
                {
                    row.Level = Math.Max(row.Level, e.Level);
                }

                row.SelectedShipId = e.SelectedShipId;
                row.UnlockedShipsJson = unlocksJson;
                row.LastUpdatedSec = e.LastUpdatedSec;
            }

            await _db.SaveChangesAsync();
            return Created($"/api/scores/player/{e.Username}/best", new { ok = true });
        }
    }
}
