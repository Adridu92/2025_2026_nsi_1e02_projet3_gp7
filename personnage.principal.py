using UnityEngine;

public class PlayerController : MonoBehaviour
{
    [Header("Movement")]
    public float speed = 5f;
    public float jumpForce = 7f;
    private Rigidbody2D rb;
    private bool isGrounded;

    public Transform groundCheck;
    public float groundCheckRadius = 0.1f;
    public LayerMask groundLayer;

    [Header("Attack")]
    public Transform attackPoint;
    public float attackRange = 0.7f;
    public LayerMask enemyLayer;
    public int damage = 1;
    public float attackCooldown = 0.5f;
    private float attackTimer = 0f;

    private Animator anim;
    private bool facingRight = true;

    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        anim = GetComponent<Animator>();
    }

    void Update()
    {
        Move();
        Jump();
        Attack();
    }

    void Move()
    {
        float move = Input.GetAxis("Horizontal");

        rb.linearVelocity = new Vector2(move * speed, rb.linearVelocity.y);

        // animation
        if (anim != null)
            anim.SetFloat("Speed", Mathf.Abs(move));

        // flip
        if (move > 0 && !facingRight)
            Flip();
        else if (move < 0 && facingRight)
            Flip();
    }

    void Jump()
    {
        isGrounded = Physics2D.OverlapCircle(groundCheck.position, groundCheckRadius, groundLayer);

        if (Input.GetButtonDown("Jump") && isGrounded)
        {
            rb.linearVelocity = new Vector2(rb.linearVelocity.x, jumpForce);

            if (anim != null)
                anim.SetTrigger("Jump");
        }
    }

    void Attack()
    {
        attackTimer -= Time.deltaTime;

        if (Input.GetKeyDown(KeyCode.Space) && attackTimer <= 0f)
        {
            if (anim != null)
                anim.SetTrigger("Attack");

            Collider2D[] hits = Physics2D.OverlapCircleAll(attackPoint.position, attackRange, enemyLayer);

            foreach (Collider2D hit in hits)
            {
                // marche pour mobs + boss
                if (hit.GetComponent<SwordMobAI>() != null)
                    hit.GetComponent<SwordMobAI>().TakeDamage(damage);

                if (hit.GetComponent<BossAI>() != null)
                    hit.GetComponent<BossAI>().TakeDamage(damage);
            }

            attackTimer = attackCooldown;
        }
    }

    void Flip()
    {
        facingRight = !facingRight;

        Vector3 scale = transform.localScale;
        scale.x *= -1;
        transform.localScale = scale;
    }

    void OnDrawGizmosSelected()
    {
        if (attackPoint == null) return;

        Gizmos.color = Color.red;
        Gizmos.DrawWireSphere(attackPoint.position, attackRange);
    }
}


# SCRIPT DE VIE DU JOUEUR

using UnityEngine;

public class PlayerHealth : MonoBehaviour
{
    public int maxHealth = 5;
    private int currentHealth;

    private Animator anim;

    void Start()
    {
        currentHealth = maxHealth;
        anim = GetComponent<Animator>();
    }

    public void TakeDamage(int dmg)
    {
        currentHealth -= dmg;

        if (anim != null)
            anim.SetTrigger("Hit");

        Debug.Log("HP: " + currentHealth);

        if (currentHealth <= 0)
        {
            Die();
        }
    }

    public void Heal(int amount)
    {
        currentHealth += amount;

        if (currentHealth > maxHealth)
            currentHealth = maxHealth;

        Debug.Log("HP: " + currentHealth);
    }

    void Die()
    {
        Debug.Log("Player Dead");

        if (anim != null)
            anim.SetTrigger("Die");

        // désactive contrôle
        GetComponent<PlayerController>().enabled = false;
    }
}


#Code loot

using UnityEngine;

public class HeartPickup : MonoBehaviour
{
    public int healAmount = 1;

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.CompareTag("Player"))
        {
            PlayerHealth ph = col.GetComponent<PlayerHealth>();

            if (ph != null)
            {
                ph.Heal(healAmount);
            }

            Destroy(gameObject);
        }
    }
}