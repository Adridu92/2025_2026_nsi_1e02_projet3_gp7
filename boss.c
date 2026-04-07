using UnityEngine;
using UnityEngine.UI;
using System.Collections;

// =====================
// BOSS AI
// =====================
public class BossAI : MonoBehaviour
{
    [Header("Player")]
    public Transform player;

    [Header("Detection")]
    public float detectionRange = 10f;

    [Header("Attack")]
    public float meleeRange = 2f;
    public float baseAttackCooldown = 2f;

    private float attackCooldown;
    private float cooldownTimer;

    [Header("Health")]
    public int maxHealth = 30;
    private int currentHealth;
    private bool isDead = false;
    private bool hasSummoned = false;

    [Header("UI")]
    public Slider healthBar;

    [Header("Prefabs")]
    public GameObject fireballPrefab;
    public GameObject meleeHitboxPrefab;
    public GameObject skeletonPrefab;

    [Header("Points")]
    public Transform firePoint;
    public Transform meleePoint;
    public Transform leftSpawn;
    public Transform rightSpawn;

    private Animator anim;

    void Start()
    {
        currentHealth = maxHealth;
        anim = GetComponent<Animator>();
        attackCooldown = baseAttackCooldown;

        if (healthBar != null)
        {
            healthBar.maxValue = maxHealth;
            healthBar.value = currentHealth;
        }
    }

    void Update()
    {
        if (isDead || player == null) return;

        float distance = Vector2.Distance(transform.position, player.position);

        // Détection du joueur
        if (distance > detectionRange) return;

        cooldownTimer -= Time.deltaTime;

        UpdatePhase();

        if (cooldownTimer <= 0f)
        {
            // Invocation à 50% HP UNE SEULE FOIS
            if (!hasSummoned && currentHealth <= maxHealth / 2)
            {
                StartCoroutine(Summon());
                hasSummoned = true;
            }
            else
            {
                if (distance <= meleeRange)
                    StartCoroutine(MeleeAttack());
                else
                    StartCoroutine(RangedAttack());
            }

            cooldownTimer = attackCooldown;
        }
    }

    // =====================
    // PHASE SYSTEM
    // =====================
    void UpdatePhase()
    {
        if (currentHealth <= maxHealth / 2)
        {
            attackCooldown = baseAttackCooldown * 0.5f; // phase 2 plus rapide
        }
        else
        {
            attackCooldown = baseAttackCooldown;
        }
    }

    // =====================
    // ATTACKS
    // =====================

    IEnumerator RangedAttack()
    {
        anim.SetTrigger("Ranged");
        yield return new WaitForSeconds(0.4f);

        Instantiate(fireballPrefab, firePoint.position, Quaternion.identity);
    }

    IEnumerator MeleeAttack()
    {
        anim.SetTrigger("Melee");
        yield return new WaitForSeconds(0.3f);

        GameObject hit = Instantiate(meleeHitboxPrefab, meleePoint.position, Quaternion.identity);
        Destroy(hit, 0.2f);
    }

    IEnumerator Summon()
    {
        anim.SetTrigger("Summon");
        yield return new WaitForSeconds(0.5f);

        Instantiate(skeletonPrefab, leftSpawn.position, Quaternion.identity);
        Instantiate(skeletonPrefab, rightSpawn.position, Quaternion.identity);
    }

    // =====================
    // DAMAGE
    // =====================
    public void TakeDamage(int dmg)
    {
        if (isDead) return;

        currentHealth -= dmg;

        if (healthBar != null)
            healthBar.value = currentHealth;

        if (anim != null)
            anim.SetTrigger("Hit");

        if (currentHealth <= 0)
        {
            Die();
        }
    }

    void Die()
    {
        isDead = true;

        anim.SetTrigger("Die");

        this.enabled = false;

        Destroy(gameObject, 2f);
    }
}

// =====================
// FIREBALL
// =====================
public class Fireball : MonoBehaviour
{
    public float speed = 6f;
    public int damage = 1;

    void Update()
    {
        transform.Translate(Vector2.right * speed * Time.deltaTime);
    }

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.CompareTag("Player"))
        {
            PlayerHealth ph = col.GetComponent<PlayerHealth>();
            if (ph != null)
            {
                ph.TakeDamage(damage);
            }

            Destroy(gameObject);
        }
    }
}

// =====================
// MELEE HITBOX
// =====================
public class MeleeHitbox : MonoBehaviour
{
    public int damage = 1;

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.CompareTag("Player"))
        {
            PlayerHealth ph = col.GetComponent<PlayerHealth>();
            if (ph != null)
            {
                ph.TakeDamage(damage);
            }
        }
    }
}

// =====================
// SKELETON
// =====================
public class Skeleton : MonoBehaviour
{
    public float speed = 2f;

    void Update()
    {
        transform.Translate(Vector2.left * speed * Time.deltaTime);
    }
}

// =====================
// PLAYER HEALTH
// =====================
public class PlayerHealth : MonoBehaviour
{
    public int maxHealth = 10;
    private int currentHealth;

    void Start()
    {
        currentHealth = maxHealth;
    }

    public void TakeDamage(int dmg)
    {
        currentHealth -= dmg;
        Debug.Log("Player HP: " + currentHealth);

        if (currentHealth <= 0)
        {
            Die();
        }
    }

    void Die()
    {
        Debug.Log("Player died");
    }
}