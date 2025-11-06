using Microsoft.EntityFrameworkCore;
using RestAPI.Models;

namespace RestAPI.Data
{
    public class LeaderboardDbContext : DbContext
    {
        public LeaderboardDbContext(DbContextOptions<LeaderboardDbContext> options) : base(options) { }

        public DbSet<HighScore> HighScores => Set<HighScore>();

        protected override void OnModelCreating(ModelBuilder b)
        {
            // Store DateTimeOffset as UTC DateTime so SQLite can sort/filter it
            b.Entity<HighScore>()
            .Property(x => x.AchievedAt)
            .HasColumnType("TEXT");



            b.Entity<HighScore>().Property(x => x.PlayerName).IsRequired();
            b.Entity<HighScore>().HasIndex(x => x.PlayerName);
        }
    }
}
